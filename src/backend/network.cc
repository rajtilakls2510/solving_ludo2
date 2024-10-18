#ifndef NETWORK_CC
#define NETWORK_CC

#include "network.hpp"
// #include <chrono>
// #include <thread>

void print_size(torch::Tensor x){
    std::cout << "Tensor[" ;
    for(auto& size: x.sizes())
        std::cout << size << ", ";
    std::cout << "]" << std::endl;
}

torch::Device getDevice(std::string device) {
    return device == "cuda" ? torch::kCUDA : torch::kCPU;
}

// ========= Multi Head Attention ==================

MultiHeadAttentionImpl::MultiHeadAttentionImpl(int embed_dim, int num_heads, std::string device, float dropout) {
    this->device = device;
    
    if (embed_dim % num_heads != 0)
        throw std::runtime_error("embed_dim is not divisible by num_heads.");

    this->embed_dim = embed_dim;
    this->num_heads = num_heads;
    this->head_dim = embed_dim / num_heads;

    // Linear layers for query, key and value
    this->qkv_proj = register_module("qkv_proj", torch::nn::Linear(torch::nn::LinearOptions(embed_dim, 3 * embed_dim).bias(false)));
    
    // Linear layer for the output
    this->out_proj = register_module("out_proj", torch::nn::Linear(torch::nn::LinearOptions(embed_dim, embed_dim).bias(false)));

    this->dropout = register_module("dropout", torch::nn::Dropout(torch::nn::DropoutOptions().p(dropout)));
    this->to(getDevice(this->device));
}

torch::Tensor MultiHeadAttentionImpl::forward(torch::Tensor x) {
    x = x.to(getDevice(this->device));
    int batch_size = x.sizes()[0];
    int seq_len = x.sizes()[1];

    auto qkv = this->qkv_proj(x);    // T(B,S,D) x T(D, 3D) -> T(B,S,D,3D)
    qkv = qkv.view({batch_size, seq_len, this->num_heads, 3 * this->head_dim}); // T(B,S,3D) -> T(B,S,H,3K) [D=N*K]
    // T(B,S,H,3K) -> T(B,S,H,K), T(B,S,H,K), T(B,S,H,K)
    auto q = qkv.slice(3, 0, head_dim);
    auto k = qkv.slice(3, head_dim, 2*head_dim);
    auto v = qkv.slice(3, 2*head_dim, 3*head_dim);

    auto atten_scores = torch::einsum("bshk,bshk->bhs", {q, k}) / std::sqrt(head_dim); // T(B,S,H,K) eisum T(B,S,H,K) -> T(B,H,S)
    auto atten_weights = this->dropout(torch::softmax(atten_scores, -1));
    auto atten_out = torch::einsum("bhs,bshk->bshk", {atten_weights, v}); // T(B,H,S) einsum T(B,S,H,K) -> T(B,S,H,K) 
    
    atten_out = atten_out.reshape({batch_size, seq_len, this->embed_dim}); 
    return this->out_proj(atten_out);
}

// ================== Feed-Forward Network ==============
FFNImpl::FFNImpl(int embed_dim, std::string device, float dropout) {
    this->device = device;
    this->embed_dim = embed_dim;

    this->fc1 = register_module("fc1", torch::nn::Linear(torch::nn::LinearOptions(embed_dim, embed_dim)));
    this->fc2 = register_module("fc2", torch::nn::Linear(torch::nn::LinearOptions(embed_dim, embed_dim)));
    this->dropout = register_module("dropout", torch::nn::Dropout(torch::nn::DropoutOptions().p(dropout)));
    this->to(getDevice(this->device));
}

torch::Tensor FFNImpl::forward(torch::Tensor x) {
    x = x.to(getDevice(this->device));
    x = this->dropout(torch::nn::functional::relu(this->fc1(x)));
    return torch::nn::functional::relu(this->fc2(x));
}

// ================= Encoder Block =======================
EncoderBlockImpl::EncoderBlockImpl(int embed_dim, int num_heads, std::string device, float dropout) {
    this->device = device;
    
    this->mha = register_module("mha", MultiHeadAttention(embed_dim, num_heads, device, dropout));
    this->ffn = register_module("ffn", FFN(embed_dim, device, dropout));
    this->ln1 = register_module("ln1", torch::nn::LayerNorm(torch::nn::LayerNormOptions({embed_dim})));
    this->ln2 = register_module("ln2", torch::nn::LayerNorm(torch::nn::LayerNormOptions({embed_dim})));
    this->to(getDevice(this->device));
}

torch::Tensor EncoderBlockImpl::forward(torch::Tensor x) {
    x = x.to(getDevice(this->device));
    x = x + mha(x);
    x = this->ln1(x);
    x = x + ffn(x);
    return this->ln2(x);
}

// ================ Input Block ===========================
InputBlockImpl::InputBlockImpl(int input_dim, int embed_dim, std::string device, int max_len) {
    this->device = device;

    this->embedding = register_module("embedding", torch::nn::Linear(torch::nn::LinearOptions(input_dim, embed_dim).bias(false)));
    // Create a 2D tensor with shape [max_len, embed_dim]
    pe = torch::zeros({max_len, embed_dim});
    
    // Calculate the positional encodings
    for (int pos = 0; pos < max_len; ++pos) {
        for (int i = 0; i < embed_dim; i += 2) {
            pe[pos][i] = std::sin(pos / std::pow(10000, (float)i / embed_dim));
            if (i + 1 < embed_dim) {
                pe[pos][i + 1] = std::cos(pos / std::pow(10000, (float)(i + 1) / embed_dim));
            }
        }
    }
    
    // Add a batch dimension by unsqueezing at position 0
    pe = pe.unsqueeze(0); // Shape: [1, max_len, embed_dim]

    // Register the positional encoding as a buffer, so it’s not updated during training
    register_buffer("pe", pe);

    this->to(getDevice(this->device));
}

torch::Tensor InputBlockImpl::forward(torch::Tensor x) {
    x = x.to(getDevice(this->device));
    x = this->embedding(x);
    auto seq_len = x.size(1);
    x = x + pe.slice(1, 0, seq_len);
    return x;
}

// =============== Value Net ================================
ValueNetImpl::ValueNetImpl(int input_dim, int embed_dim, int value_head_dim, std::string device, float dropout) {
    this->device = device;
    this->input_block = register_module("input_block", InputBlock(input_dim, embed_dim, device));
    this->en1 = register_module("en1", EncoderBlock(embed_dim, 4, device, dropout));
    this->en2 = register_module("en2", EncoderBlock(embed_dim, 4, device, dropout));
    this->en3 = register_module("en3", EncoderBlock(embed_dim, 4, device, dropout));
    this->vfc1 = register_module("vfc1", torch::nn::Linear(torch::nn::LinearOptions(embed_dim, value_head_dim)));
    this->vfc2 = register_module("vfc2", torch::nn::Linear(torch::nn::LinearOptions(value_head_dim, 1)));
    this->to(getDevice(this->device));
}

torch::Tensor ValueNetImpl::forward(torch::Tensor x) {
    x = this->input_block(x);
    x = this->en1(x);
    x = this->en2(x);
    x = this->en3(x);
    x = x.mean(1);
    x = torch::nn::functional::relu(this->vfc1(x));
    x = this->vfc2(x);
    return x;
}

template <typename T>
int64_t count_parameters(const torch::nn::ModuleHolder<T>& model) {
    int64_t total_params = 0;
    // Iterate over all the parameters in the model
    for (const auto& param : model->parameters()) {
        total_params += param.numel(); // numel() gives the total number of elements in the tensor
    }
    return total_params;
}


// int main(int argc, char* argv[]) {
//     ValueNet v = ValueNet(26, 128, 1024, "cuda", 0.1);
//     std::cout << "Total Parameters: " << count_parameters(v) << std::endl;
//     torch::NoGradGuard no_grad;
//     int batch = 0;
//     while (true){
//         auto input = torch::zeros({std::stoi(argv[1]),59,26});
//         auto start = std::chrono::high_resolution_clock::now();
//         auto out = v(input);
//         auto end = std::chrono::high_resolution_clock::now();

//         std::cout << "\rBatch: " << batch << " Time : " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " us            ";
//         // std::this_thread::sleep_for(std::chrono::seconds(1));
//         batch++;
//     }
// }

#endif