#include "engine.hpp"
#include "network_utils.hpp"
#include "proto_utils.hpp"
#include <ludo.pb.h>
#include <system.pb.h>
#include <system.grpc.pb.h>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <fstream>
#include <chrono>
using namespace std::chrono;

// Engine test
int main() {

    // Testing Protobuf

    int game_num = 0;

    while (game_num < 1){
        auto start = high_resolution_clock::now();
        std::vector<std::vector<std::string>> colours_config = {
            {"red", "yellow"}, 
            {"blue", "green"}
        };
        std::shared_ptr<GameConfig> config = std::make_shared<GameConfig>(colours_config);
        Ludo game(config);
        game.reset();

        // Logging
        ludo::GameProto game_proto; 
        ludo::ConfigProto* config_proto = game_proto.mutable_config();
        config_to_proto(config, config_proto);
        
        while(!game.state->game_over) {

            std::cout << game.state->repr();
            std::cout << get_state_tensor_repr(game.state, game.model->get_config());
            // Logging State
            ludo::StateProto* state_proto = game_proto.add_states();
            state_to_proto(game.state, state_proto);

            std::vector<Move> moves = game.model->all_possible_moves(game.state);
            // std::cout << "Moves: [\n";
            // for (auto move : moves) {
            //     std::cout << "\t" << move.repr() << std::endl;
            // }
            // std::cout << "]\n";
            Move move_taken;
            if (moves.size() > 0)
                move_taken = moves[game.model->throw_gen->get_randint(0, moves.size())];
            game.turn(move_taken, game.state->last_move_id + 1);
            std::cout << "Move taken: " << move_taken.repr() << std::endl;

            // Logging move
            ludo::MoveProto* move_proto = game_proto.add_moves();
            move_to_proto(move_taken, move_proto);

        }
        std::cout << game.state->repr();
        std::cout << get_state_tensor_repr(game.state, game.model->get_config());
        // Logging final State
        ludo::StateProto* state_proto = game_proto.add_states();
        state_to_proto(game.state, state_proto);

        // Logging winner
        game_proto.set_winner(game.winner);

        // Saving game proto to file
        std::fstream output("game"+std::to_string(game_num)+".pb", std::ios::out | std::ios::trunc | std::ios::binary);
        if (!game_proto.SerializeToOstream(&output))
            std::cerr << "Couldn't save game file" << std::endl;
        auto stop = high_resolution_clock::now();
        std::cout << "Length: " << game.state->last_move_id << std::endl;
        std::cout << "Winner: " << game.winner << std::endl;
        std::cout << "Game Generation Time: " << duration_cast<milliseconds>(stop - start).count() << " ms" << std::endl;

        game_num++;
    }
    

    // // Testing Saved game file
    // std::fstream input("game0.pb", std::ios::in | std::ios::binary);
    // ludo::GameProto game_proto;
    // if (!game_proto.ParseFromIstream(&input))
    //     std::cerr << "Couldn't load game file" << std::endl;

    // std::cout << proto_to_config(game_proto.mutable_config())->repr() << std::endl;

    // for (int m = 0; m < game_proto.moves_size(); m++) {
    //     std::cout << proto_to_state(game_proto.mutable_states(m))->repr();
    //     std::cout << "Move Taken: " << proto_to_move(game_proto.mutable_moves(m)).repr() << std::endl;
    // }
    // std::cout << proto_to_state(game_proto.mutable_states(game_proto.moves_size()))->repr();
    // std::cout << "Winner: " << game_proto.winner() << std::endl;

    // Testing Game Save
    // fs::path run_dir (argv[1]);
    // auto channel = grpc::CreateChannel("0.0.0.0:50051", grpc::InsecureChannelCredentials());
    // std::unique_ptr<alphaludo::GamesManager::Stub> stub = alphaludo::GamesManager::NewStub(channel);

    // {
    // grpc::ClientContext context;
    // alphaludo::FileName request;
    // request.set_file("game0.pb");
    // ::google::protobuf::Empty response;
    // grpc::Status status = stub->Save(&context, request, &response);
    // std::cout << "status : " << status.error_code() << std::endl;
    // std::cout << "status : " << status.error_message() << std::endl;
    // }
    // {
    // grpc::ClientContext context;
    // ::google::protobuf::Empty request;
    // alphaludo::FileNames response;
    // grpc::Status status = stub->GetAll(&context, request, &response);
    // std::cout << "status : " << status.error_code() << std::endl;
    // std::cout << "status : " << status.error_message() << std::endl;
    // for(int i = 0; i < response.files_size(); i++)
    //     std::cout << response.files(i) << std::endl;
    // }
    // {
    // grpc::ClientContext context;
    // alphaludo::FileName request;
    // request.set_file("game0.pb");
    // ludo::GameProto response;
    // grpc::Status status = stub->Get(&context, request, &response);
    // std::cout << "status : " << status.error_code() << std::endl;
    // std::cout << "status : " << status.error_message() << std::endl;
    // std::cout << proto_to_state(response.mutable_states(0))->repr();
    // }

    return 0;
}