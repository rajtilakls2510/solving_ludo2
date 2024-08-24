#include "engine.hpp"
#include <iostream>
#include <chrono>
using namespace std::chrono;

// Engine test
int main() {

    int game_num = 0;

    while (game_num < 100){
        auto start = high_resolution_clock::now();
        std::vector<std::vector<std::string>> colours_config = {
            {"red", "yellow"}, 
            {"blue", "green"}
        };
        Ludo game(std::make_shared<GameConfig>(colours_config));
        game.reset();
        
        while(!game.state->game_over) {
            // std::cout << game.state->repr();
            std::vector<Move> moves = game.model->all_possible_moves(game.state);
            // std::cout << "Moves: [\n";
            // for (auto move : moves) {
            //     std::cout << "\t" << move.repr() << std::endl;
            // }
            // std::cout << "]\n";
            if (moves.size() > 0)
                game.turn(moves[game.model->throw_gen->get_randint(0, moves.size())], game.state->last_move_id + 1);
            else
                game.turn(Move(), game.state->last_move_id + 1);
        }
        // std::cout << game.state->repr();
        
        auto stop = high_resolution_clock::now();
        std::cout << "Length: " << game.state->last_move_id << std::endl;
        std::cout << "Winner: " << game.winner << std::endl;
        std::cout << "Game Generation Time: " << duration_cast<milliseconds>(stop - start).count() << " ms" << std::endl;
        // std::cout << "Game num: " << game_num << std::endl;
        game_num++;
    }
    return 0;
}