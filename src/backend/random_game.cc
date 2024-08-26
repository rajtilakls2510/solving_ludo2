#include "engine.hpp"
#include "proto_utils.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
using namespace std::chrono;

// Engine test
int main() {

    // Testing Protobuf

    int game_num = 0;

    while (game_num < 100){
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
        std::fstream output("game"+std::to_string(game_num)+".pb", std::ios::out | std::ios::trunc | std::ios::binary);
        if (!game_proto.SerializeToOstream(&output))
            std::cerr << "Couldn't save game file" << std::endl;
        auto stop = high_resolution_clock::now();
        std::cout << "Length: " << game.state->last_move_id << std::endl;
        std::cout << "Winner: " << game.winner << std::endl;
        std::cout << "Game Generation Time: " << duration_cast<milliseconds>(stop - start).count() << " ms" << std::endl;

        game_num++;
    }
    return 0;
}