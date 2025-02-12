#ifndef LEARNER_CC
#define LEARNER_CC

#include "engine.hpp" 
#include "network.hpp"
#include "network_utils.hpp"
#include "proto_utils.hpp"
#include "threading_utils.hpp"
#include <ludo.pb.h>
#include <torch/torch.h>
#include <system.grpc.pb.h>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <chrono>
#include <filesystem>
#include <random>
#include <memory>
#include <fstream>
#include <stdexcept>
namespace fs = std::filesystem;
using namespace std::chrono;


struct Batch {
    torch::Tensor state_repr;
    torch::Tensor target;
    Batch(torch::Tensor state_repr, torch::Tensor target) : state_repr(state_repr), target(target) {};
};

// ============================ Games Cache ===============================

class GamesCache { // Simple circular FIFO cache
public:

    GamesCache(int max_size) : max_size(max_size), current(0) {
        this->cache = new ludo::GameProto*[this->max_size](); // Allocate cache space and initialize with (nullptr)
        for (int i = 0; i < this->max_size; i++) {
            this->cache[i] = nullptr;
        }
    }

    void add(ludo::GameProto* game) {

        if (cache[this->current])
            delete cache[this->current];
        
        cache[this->current] = game;
        this->current = (this->current + 1) % this->max_size;
    }

    ludo::GameProto* get(int i) {
        return this->cache[i];
    }

    ~GamesCache() {

        // De-allocate cache and all loaded games
        for (int i = 0; i < max_size; i++)
            if(this->cache[i])
                delete this->cache[i];
        delete[] this->cache;
    }
    
    int getSize() {
        return this->max_size;
    }

private:
    int max_size;
    int current;
    ludo::GameProto** cache{nullptr};

};
using CachePtr = std::shared_ptr<GamesCache>;
using PrefetchQPtr = std::shared_ptr<TSQueue<Batch>>;


void cache_games(std::atomic<bool>& stop_flag, CachePtr cache, fs::path run_dir) {

    auto channel = grpc::CreateChannel("0.0.0.0:50051", grpc::InsecureChannelCredentials());
    std::unique_ptr<alphaludo::GamesManager::Stub> games_stub = alphaludo::GamesManager::NewStub(channel);

    while (!stop_flag.load()) {
        
        grpc::ClientContext context;
        ::google::protobuf::Empty request;
        alphaludo::FileName response;
        grpc::Status status = games_stub->GetRandom(&context, request, &response);
        if (!status.ok()) {
            std::cout << "status : " << status.error_code() << std::endl;
            std::cout << "status : " << status.error_message() << std::endl;
            continue;
        }
        fs::path game_file_path = run_dir / "games" / response.file();

        ludo::GameProto* game_proto = new ludo::GameProto();
        std::fstream input(game_file_path, std::ios::in | std::ios::binary);
        if (!game_proto->ParseFromIstream(&input)) {
            std::cerr << "Couldn't parse game: " + game_file_path.string() << std::endl;
            delete game_proto;
            continue;
        }

        std::cout << "Loaded game: " + game_file_path.string() << std::endl;
        cache->add(game_proto);     // game_proto ownership transferred to cache

        std::this_thread::sleep_for(seconds(1));
    }

}

// ================================ Batch Prefetch ===============================

torch::Tensor get_pawn_permutation(std::mt19937& g) {
    // Create a 4x4 permutation matrix for a color's pawns
    torch::Tensor perm = torch::zeros({4, 4});
    std::array<int, 4> indices = {0, 1, 2, 3};

    // Shuffle indices to generate a random permutation
    std::shuffle(indices.begin(), indices.end(), g);

    // Set entries to create the permutation matrix
    for (int i = 0; i < 4; ++i) {
        perm[i][indices[i]] = 1;
    }
    return perm;
}

void prefetch(std::atomic<bool>& stop_flag, CachePtr cache, PrefetchQPtr pq, int batch_size, std::mt19937& g) {
    
    // std::this_thread::sleep_for(seconds(1)); // Waiting for cache thread to load atleast one game
    while(!stop_flag.load()) {
        // auto start = high_resolution_clock::now();
        torch::Tensor state_repr_b = torch::zeros({batch_size, 59, 26}, torch::kFloat32).to(torch::kCUDA);
        torch::Tensor target_b = torch::zeros({batch_size, 1}, torch::kFloat32).to(torch::kCUDA);

        for (int i = 0; i < batch_size; i++) {
            
            // Select a game from cache
            std::uniform_int_distribution<> distrib(0, cache->getSize()-1);
            bool invalid_game_found = true;
            int cg = distrib(g);
            while (invalid_game_found) {
                cg = distrib(g);
                if (cache->get(cg))
                    invalid_game_found = false;
            }
            ludo::GameProto* chosen_game = cache->get(cg);

            std::shared_ptr<GameConfig> config = proto_to_config(chosen_game->mutable_config());

            // Select a random state
            std::uniform_int_distribution<> distrib2(0, chosen_game->states_size()-1);
            int cs = distrib2(g);
            StatePtr state = proto_to_state(chosen_game->mutable_states(cs));
            torch::Tensor state_repr = get_state_tensor_repr(state, config);
            
            // Apply turn augmentation 
            int winner = chosen_game->winner();
            std::uniform_int_distribution<> distrib3(0, state->n_players-1);
            float cp = (float) distrib3(g);
            state_repr.index_put_({torch::indexing::Slice(), 20}, cp);

            // Apply pawn augmentation
            torch::Tensor permutation_array = torch::eye(26, torch::kFloat32);
            permutation_array.index_put_({torch::indexing::Slice(0, 4), torch::indexing::Slice(0, 4)}, get_pawn_permutation(g));      // Red
            permutation_array.index_put_({torch::indexing::Slice(4, 8), torch::indexing::Slice(4, 8)}, get_pawn_permutation(g));      // Green
            permutation_array.index_put_({torch::indexing::Slice(8, 12), torch::indexing::Slice(8, 12)}, get_pawn_permutation(g));    // Yellow
            permutation_array.index_put_({torch::indexing::Slice(12, 16), torch::indexing::Slice(12, 16)}, get_pawn_permutation(g));  // Blue
            state_repr = torch::matmul(state_repr.to(torch::kCUDA), permutation_array.to(torch::kCUDA));

            // Generate reward
            float reward = (winner == cp) ? 1.0 : -1.0;

            // Add record to batch
            state_repr_b.index_put_({i}, state_repr);
            target_b[i][0] = reward;
        }
        // auto end = high_resolution_clock::now();
        // std::cout << "Batch creation time : " << duration_cast<microseconds>(end - start).count() << " us\n";
        
        // Add batch to prefetch_queue
        pq->push(Batch(state_repr_b, target_b));
    }
}

class Learner {
public:
    Learner(fs::path run_dir, int cache_size, int batch_size, float save_after_hours) : g(std::random_device{}()), run_dir(run_dir), save_after_hours(save_after_hours) {
        
        std::cout << "Connecting to Manager..." << std::endl;
        auto channel = grpc::CreateChannel("0.0.0.0:50051", grpc::InsecureChannelCredentials());
        std::unique_ptr<alphaludo::GamesManager::Stub> games_stub = alphaludo::GamesManager::NewStub(channel);
        std::unique_ptr<alphaludo::PlayerManager::Stub> players_stub = alphaludo::PlayerManager::NewStub(channel);
        
        std::cout << "Starting Learner..." << std::endl;

        // Load latest checkpoint
        v_net = ValueNet(26, 128, 1024, "cuda", 0.1);
        optimizer = std::make_shared<torch::optim::Adam>(v_net->parameters(), torch::optim::AdamOptions(1e-4)); // Learning rate: 1e-4
        fs::path players_path = run_dir / "players";
        grpc::ClientContext context;
        ::google::protobuf::Empty request;
        alphaludo::FileName response;
        grpc::Status status = players_stub->GetLatest(&context, request, &response);
        if (!status.ok()) {
            std::cout << "status : " << status.error_code() << std::endl;
            std::cout << "status : " << status.error_message() << std::endl;
            throw std::runtime_error("Couldn't find network path");
        }
        else { 
            torch::load(v_net, (players_path / ("network_" + response.file())).string());
            v_net->train();    
            torch::load(*optimizer, (players_path / ("optim_" + response.file())).string());
            std::cout << "Loaded " << (players_path / ("network_" + response.file())).string() << std::endl;
        }
        
        // Initialize GamesCache and Prefetch Queue
        this->games_cache = std::make_shared<GamesCache>(cache_size);
        this->prefetch_queue = std::make_shared<TSQueue<Batch>>();

        // Start GameCache and Prefetch thread
        this->games_cache_thread = std::thread(cache_games, std::ref(this->stop_flag), this->games_cache, run_dir);
        this->prefetch_thread = std::thread(prefetch, std::ref(this->stop_flag), this->games_cache, this->prefetch_queue, batch_size, std::ref(this->g));

    }

    torch::Tensor train_step(Batch batch) {

        this->optimizer->zero_grad();
        torch::Tensor pred = this->v_net(batch.state_repr);
        torch::Tensor loss = torch::nn::functional::mse_loss(pred, batch.target);
        loss.backward();
        this->optimizer->step();

        return loss;
    }

    void learn(int initial_batch, int final_batch) {
        
        auto channel = grpc::CreateChannel("0.0.0.0:50051", grpc::InsecureChannelCredentials());
        std::unique_ptr<alphaludo::PlayerManager::Stub> players_stub = alphaludo::PlayerManager::NewStub(channel);
        
        auto start = high_resolution_clock::now();
        for (int i = initial_batch; i < final_batch; i++) {
            // std::cout << "Queue size : " << this->prefetch_queue->size() << "\n"; 
            auto popped = this->prefetch_queue->pop(); // Pop a batch from prefetch queue. Wait if there is no batch pushed yet

            if (popped.has_value()) {
                torch::Tensor loss = train_step(*popped);
                auto end = high_resolution_clock::now();

                std::cout << "Train step time : " << duration_cast<seconds>(end - start).count() << " s / " << save_after_hours * 60 *60 << " s Loss : " << loss.item<float>() << "\n";
                
                // If a desired amount of time has passed, save a checkpoint and notify players manager
                if (duration_cast<seconds>(end - start).count() > save_after_hours * 60 * 60) {
                    std::cout << "Saving Checkpoint..." << std::endl;
                    // Save checkpoint
                    fs::path players_dir = run_dir / "players";
                    std::string checkpoint_name = get_formatted_time(std::chrono::system_clock::now()) + ".pth";
                    torch::save(v_net, (players_dir / ("network_" + checkpoint_name)).string());
                    torch::save(*optimizer, (players_dir / ("optim_" + checkpoint_name)).string());
                    
                    // Notify Player manager about the newly saved game
                    grpc::ClientContext context;
                    ::google::protobuf::Empty response;
                    alphaludo::FileName request;
                    request.set_file(checkpoint_name);
                    grpc::Status status = players_stub->Save(&context, request, &response);
                    if (!status.ok()) {
                        std::cout << "status : " << status.error_code() << std::endl;
                        std::cout << "status : " << status.error_message() << std::endl;
                        std::cout << "Couldn't save checkpoint" << std::endl;
                    }
                    start = high_resolution_clock::now();
                }
            }
        }
    }

    ~Learner() {
        this->stop_flag.store(true);
        this->games_cache_thread.join();
        this->prefetch_thread.join();
        this->prefetch_queue->clear();   
    }

private:
    std::thread games_cache_thread;
    std::thread prefetch_thread;
    CachePtr games_cache;
    PrefetchQPtr prefetch_queue; // a thread-safe queue for storing prefetched batches
    ValueNet v_net{nullptr};
    std::shared_ptr<torch::optim::Adam> optimizer{nullptr};
    std::atomic<bool> stop_flag{false};
    std::mt19937 g;
    float save_after_hours{1.0};
    fs::path run_dir;
};


int main(int argc, char* argv[]) {
    
    if (argc >= 2) {
        fs::path run_dir (argv[1]);
        Learner learner(run_dir, 100, 64, 8);
        learner.learn(0, 10000);
    }
    else {
        std::cerr << "Arg1 : run directory" << std::endl;
    }
    return 0;
}



#endif