#ifndef LUDO_ENGINE_HPP
#define LUDO_ENGINE_HPP

#include <memory>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>

class Block {
public:
    int pawns;
    short pos;
    bool rigid;
};

class State {
public:
    short n_players;
    bool game_over;
    short current_player;
    short num_more_moves;
    short dice_roll;
    short last_move_id;
    int* pawn_pos;
    short num_blocks;
    Block blocks[16];

    StatePtr copy();    // Copies the state
};

using StatePtr = std::shared_ptr<State>;

class Move {
public:
    short n_rolls;
    int* pawns;
    short* current_positions;
    short* destinations;
};

using MovePtr = std::shared_ptr<Move>;

class GameConfig {
    // Configuration of the game. For now, contains only the mapping between player and pawn colour.
public:
    short n_players;
    short* player_colours;  // Index: player (0-indexed), Value: colour (1-indexed)
    short colour_player[5];    // Index: colour, Value: Player
    GameConfig(std::vector<std::vector<std::string>> player_colour_choices);
    ~GameConfig();
    std::string repr();     // Returns a printable representation of the current configuration
};

class LudoModel {
private:
    short* stars;   // Array that defines the positions of stars
    short* final_pos;   // Array that defines the positions of home for each colour
    short* colour_tracks;   // Sequence of positions (tracks) which pawns follow for each colour 
    short* colour_bases;    // Base positions for each colour
    std::shared_ptr<GameConfig> config;    // Config object of the game
public:
    LudoModel(std::shared_ptr<GameConfig> config);
    ~LudoModel();

    std::vector<StatePtr> generate_next_states(StatePtr state, MovePtr move);   // Returns all next states given current state and move
    std::vector<MovePtr> all_possible_moves(StatePtr state);    // Returns all possible moves for a given state 

};

#endif