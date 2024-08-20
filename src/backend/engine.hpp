#ifndef LUDO_ENGINE_HPP
#define LUDO_ENGINE_HPP

#include <memory>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <random>
#include <cstring>

class Block {
public:
    int pawns;
    short pos;
    bool rigid;

    Block() : pawns(0), pos(0), rigid(false) {};
    Block(int pawns, short pos, bool rigid) : pawns(pawns), pos(pos), rigid(rigid) {};
    ~Block() {};

};

class State {
public:
    short n_players;    // Number of players
    bool game_over;     // Whether the game is over or not
    short current_player;   // Who is the current player
    short num_more_moves;   // Number of moves left for the current player
    short dice_roll;        // All the dice throws added up from when the turn began
    short last_move_id;     // Move id of the last move taken
    int* player_pos_pawn;   // For each player, for each position, the pawns that are present
    short num_blocks;       // Number of blocks present in the tracks
    Block blocks[16];       // blocks present in the tracks
    int previous_pawn_pos;  // Stack to hold pawn and their previous position before they were moved. 
    // (This is kept to return the pawns in their original positions when there are 3 dice 6's)

    State(short n_players);
    ~State();
    
    std::shared_ptr<State> copy();    // Copies the state
    std::string repr();
};

using StatePtr = std::shared_ptr<State>;

class Move {
public:
    int pawn;       // Pawn that is being moved
    short current_position;     // Current position of the pawn
    short destination;          // Destination of the pawn
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

class ThrowGenerator {
public:
    ThrowGenerator() : gen(std::random_device{}()) {}

    int get_throw() {
        std::uniform_int_distribution<> distrib(1,6);
        return distrib(gen);
    }

private:
    std::mt19937 gen;
};

class LudoModel {
private:
    short* stars;   // Array that defines the positions of stars
    short* final_pos;   // Array that defines the positions of home for each colour
    short* colour_tracks;   // Sequence of positions (tracks) which pawns follow for each colour 
    short* colour_bases;    // Base positions for each colour
    std::shared_ptr<GameConfig> config;    // Config object of the game

public:
    std::shared_ptr<ThrowGenerator> throw_gen;

    LudoModel(std::shared_ptr<GameConfig> config);
    ~LudoModel();

    std::vector<StatePtr> generate_next_states(StatePtr state, MovePtr move);   // Returns all next states given current state and move
    std::vector<MovePtr> all_possible_moves(StatePtr state);    // Returns all possible moves for a given state 
    StatePtr get_initial_state();       // Returns the initial state that the game starts in

};

using LudoModelPtr = std::shared_ptr<LudoModel>;

class Ludo {
public:
    StatePtr state;     // Holds the current state
    LudoModelPtr model;     // Holds the model
    short winner{-1};       // Winner of the game when the game has finished (-1: no winner yet)

    Ludo(std::shared_ptr<GameConfig> config) {
        this->model = std::make_shared<LudoModel>(config);
        state = model->get_initial_state();
        winner = -1;
    }

    void reset();

    ~Ludo () {};

};

#endif