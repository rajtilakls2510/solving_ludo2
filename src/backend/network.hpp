#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <torch/torch.h>
#include <iostream>

// ============ Creates the Value network torch neural network model using Transformers =================

void print_size(torch::Tensor x);

template <typename T>
int64_t count_parameters(const torch::nn::ModuleHolder<T>& model) {
    int64_t total_params = 0;
    // Iterate over all the parameters in the model
    for (const auto& param : model->parameters()) {
        total_params += param.numel(); // numel() gives the total number of elements in the tensor
    }
    return total_params;
}

class MultiHeadAttentionImpl : public torch::nn::Module {
public:
    MultiHeadAttentionImpl(int embed_dim, int num_heads, std::string device = "cpu", float dropout=0.0);
    torch::Tensor forward(torch::Tensor x);

    std::string device;
    int head_dim, embed_dim, num_heads;
    torch::nn::Linear qkv_proj{nullptr}, out_proj{nullptr};
    torch::nn::Dropout dropout{nullptr};
};

TORCH_MODULE(MultiHeadAttention);

class FFNImpl : public torch::nn::Module {
public:
    FFNImpl(int embed_dim, std::string device = "cpu", float dropout = 0.0);
    torch::Tensor forward(torch::Tensor x);

    std::string device;
    int embed_dim;
    torch::nn::Linear fc1{nullptr}, fc2{nullptr};
    torch::nn::Dropout dropout{nullptr};
};

TORCH_MODULE(FFN);

class EncoderBlockImpl : public torch::nn::Module {
public:
    EncoderBlockImpl(int embed_dim, int num_heads, std::string device = "cpu", float dropout = 0.0);
    torch::Tensor forward(torch::Tensor x);

    std::string device;
    MultiHeadAttention mha{nullptr};
    FFN ffn{nullptr};
    torch::nn::LayerNorm ln1{nullptr}, ln2{nullptr};
};

TORCH_MODULE(EncoderBlock);

class InputBlockImpl : public torch::nn::Module {
public:
    InputBlockImpl(int input_dim, int embed_dim, std::string device = "cpu", int max_len = 59);
    torch::Tensor forward(torch::Tensor x);

    std::string device;
    torch::nn::Linear embedding{nullptr};
    torch::Tensor pe;
};

TORCH_MODULE(InputBlock);

class ValueNetImpl : public torch::nn::Module {
public:
    ValueNetImpl(int input_dim, int embed_dim, int value_head_dim, std::string device = "cpu", float dropout = 0.0);
    torch::Tensor forward(torch::Tensor x);

    std::string device;
    InputBlock input_block{nullptr};
    EncoderBlock en1{nullptr}, en2{nullptr}, en3{nullptr};
    torch::nn::Linear vfc1{nullptr}, vfc2{nullptr};
};

TORCH_MODULE(ValueNet);

#endif