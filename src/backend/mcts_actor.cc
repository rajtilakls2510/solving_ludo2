#ifndef MCTS_ACTOR_CC
#define MCTS_ACTOR_CC


#include "engine.hpp"
#include "network.hpp"
#include "network_utils.hpp"
#include "proto_utils.hpp"
#include "mcts.hpp"
#include <ludo.pb.h>
#include <system.pb.h>
#include <torch/torch.h>
#include <system.grpc.pb.h>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <filesystem>
#include <algorithm>
#include <random> 
namespace fs = std::filesystem;
using namespace std::chrono;


void generate_game(
    fs::path run_dir, 
    std::mt19937& g, 
    std::unique_ptr<alphaludo::GamesManager::Stub>& games_stub, 
    std::unique_ptr<alphaludo::PlayerManager::Stub>& players_stub
    ) {
    std::vector<std::vector<std::string>> colours_config = {
        {"red", "yellow"}, 
        {"blue", "green"}
    };
    
    std::shuffle(colours_config.begin(), colours_config.end(), g);

    // Fetch network filenames and load
    std::vector<ValueNet> v_nets;
    for (int i = 0; i < colours_config.size(); i++) {
        ValueNet v_net = ValueNet(26, 128, 1024, "cuda", 0.1);
        fs::path netpath = run_dir / "players";
        grpc::ClientContext context;
        ::google::protobuf::Empty request;
        alphaludo::FileName response;
            
        if( i == 0) {
            grpc::Status status = players_stub->GetLatest(&context, request, &response);
            if (!status.ok()) {
                std::cout << "status : " << status.error_code() << std::endl;
                std::cout << "status : " << status.error_message() << std::endl;
            }
            else 
                netpath = netpath / ("network_" + response.file()); 
        }
        else {
            grpc::Status status = players_stub->GetRandom(&context, request, &response);
            if (!status.ok()) {
                std::cout << "status : " << status.error_code() << std::endl;
                std::cout << "status : " << status.error_message() << std::endl;
            }
            else 
                netpath = netpath / ("network_" + response.file());
        }
        std::cout << "Loading " << netpath.string() << std::endl;
        torch::load(v_net, netpath.string());
        v_nets.push_back(v_net);
        v_net->eval();
    }

    std::shared_ptr<GameConfig> config = std::make_shared<GameConfig>(colours_config);
    Ludo game(config);
    game.reset();

    // Initialize MC trees
    std::vector<MCTS*> mc_trees;
    for (int i = 0; i < colours_config.size(); i++) {
        mc_trees.push_back(new MCTS(game.state, /*player=*/i, game.model, 1.0, 3, v_nets[i]));
    }

    // Logging
    ludo::GameProto game_proto; 
    ludo::ConfigProto* config_proto = game_proto.mutable_config();
    config_to_proto(config, config_proto);
    
    
    while(!game.state->game_over) {

        ludo::StateProto* state_proto = game_proto.add_states();
        state_to_proto(game.state, state_proto);

        // Search
        mc_trees[game.state->current_player]->search(100);

        // Select move
        int selected_move_idx = mc_trees[game.state->current_player]->select_next_move();
        MoveNode* selected_move_node = mc_trees[game.state->current_player]->root->moves[selected_move_idx]; 

        // Logging move
        ludo::MoveProto* move_proto = game_proto.add_moves();
        move_to_proto(selected_move_node->move, move_proto);

        // Take move
        game.turn(selected_move_node->move, game.state->last_move_id + 1);

        // Update all trees
        for (int player = 0; player < game.state->n_players; player++) 
            mc_trees[player]->take_move(selected_move_idx, game.state);
        std::cout << "\rMove: " << game.state->last_move_id << std::flush;
    }
    
    // Logging final State
    ludo::StateProto* state_proto = game_proto.add_states();
    state_to_proto(game.state, state_proto);

    // Logging winner
    game_proto.set_winner(game.winner);

    // Saving game proto to file
    std::string game_filename = "game_" + get_formatted_time(std::chrono::system_clock::now()) + ".pb";
    std::fstream output(run_dir / "games" / game_filename, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!game_proto.SerializeToOstream(&output))
        std::cerr << "Couldn't save game file" << std::endl;
    std::cout << "\rLength: " << game.state->last_move_id << std::endl;
    std::cout << "Winner: " << game.winner << std::endl;
    // Notify Game manager about the newly saved game
    grpc::ClientContext context;
    alphaludo::FileName request;
    request.set_file(game_filename);
    ::google::protobuf::Empty response;
    grpc::Status status = games_stub->Save(&context, request, &response);
    if (!status.ok()) {
        std::cout << "status : " << status.error_code() << std::endl;
        std::cout << "status : " << status.error_message() << std::endl;
    }
}


void generate_games(fs::path run_dir) {
    
    std::cout << "Connecting to Game Manager..." << std::endl;
    auto channel = grpc::CreateChannel("0.0.0.0:50051", grpc::InsecureChannelCredentials());
    std::unique_ptr<alphaludo::GamesManager::Stub> games_stub = alphaludo::GamesManager::NewStub(channel);
    std::unique_ptr<alphaludo::PlayerManager::Stub> players_stub = alphaludo::PlayerManager::NewStub(channel);
    
    std::cout << "Generating games..." << std::endl;

    // Initialize a random number generator once
    std::random_device rd;
    std::mt19937 g(rd());
    
    while (true) {
        auto start = high_resolution_clock::now();
        
        generate_game(run_dir, g, games_stub, players_stub);
        
        auto stop = high_resolution_clock::now();
        std::cout << "Time: " << duration_cast<milliseconds>(stop - start).count() << " ms" << std::endl;
    }
}

int main(int argc, char *argv[]) {

    if (argc >= 2) {
        fs::path run_dir (argv[1]);
        generate_games(run_dir);
    }
    else {
        std::cerr << "Arg1 : run directory" << std::endl;
    }
    return 0;
}

#endif