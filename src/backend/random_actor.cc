#include "engine.hpp"
#include "proto_utils.hpp"
#include <ludo.pb.h>
#include <system.pb.h>
#include <system.grpc.pb.h>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <filesystem>
namespace fs = std::filesystem;
using namespace std::chrono;

std::string get_formatted_time() {
    // Get the current time
    auto now = std::chrono::system_clock::now();
    
    // Convert to time_t, which holds the time in seconds
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    
    // Convert to tm struct for formatting
    std::tm* localTime = std::localtime(&currentTime);
    
    // Get milliseconds
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) % 1000;

    // Use a stringstream for formatted output
    std::stringstream ss;
    ss << std::put_time(localTime, "%Y_%b_%d_%H_%M_%S_") << std::setw(3) << std::setfill('0') << ms.count();
    
    return ss.str();
}

std::string generate_game(fs::path run_dir) {
    // TODO: Randomize
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

        // std::cout << game.state->repr();
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
        // std::cout << "Move taken: " << move_taken.repr() << std::endl;

        // Logging move
        ludo::MoveProto* move_proto = game_proto.add_moves();
        move_to_proto(move_taken, move_proto);

    }
    // std::cout << game.state->repr();
    // Logging final State
    ludo::StateProto* state_proto = game_proto.add_states();
    state_to_proto(game.state, state_proto);

    // Logging winner
    game_proto.set_winner(game.winner);

    // Saving game proto to file
    std::string game_filename = "game_" + get_formatted_time() + ".pb";
    std::fstream output(run_dir / "games" / game_filename, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!game_proto.SerializeToOstream(&output))
        std::cerr << "Couldn't save game file" << std::endl;
    
    std::cout << "Length: " << game.state->last_move_id << std::endl;
    std::cout << "Winner: " << game.winner << std::endl;
    return game_filename;
}

void generate_games(fs::path run_dir, int num_games) {
    
    std::cout << "Connecting to Game Manager..." << std::endl;
    auto channel = grpc::CreateChannel("0.0.0.0:50051", grpc::InsecureChannelCredentials());
    std::unique_ptr<alphaludo::GamesManager::Stub> stub = alphaludo::GamesManager::NewStub(channel);
    
    std::cout << "Generating games..." << std::endl;
    int game = 0;
    while (game < num_games) {
        auto start = high_resolution_clock::now();
        
        std::string game_filename = generate_game(run_dir);
        
        // Notify Game manager about the newly saved game
        grpc::ClientContext context;
        alphaludo::FileName request;
        request.set_file(game_filename);
        ::google::protobuf::Empty response;
        grpc::Status status = stub->Save(&context, request, &response);
        if (!status.ok()) {
            std::cout << "status : " << status.error_code() << std::endl;
            std::cout << "status : " << status.error_message() << std::endl;
        }
        auto stop = high_resolution_clock::now();
        std::cout << "Time: " << duration_cast<milliseconds>(stop - start).count() << " ms" << std::endl;
        game++;
    }
}


int main(int argc, char *argv[]) {

    if (argc >= 3) {
        fs::path run_dir (argv[1]);
        generate_games(run_dir, std::stoi(argv[2]));
    }
    else {
        std::cerr << "Arg1 : run directory" << std::endl;
        std::cerr << "Arg2 : num_games" << std::endl;
    }
    return 0;
}