#ifndef LUDO_ENGINE_HPP
#define LUDO_ENGINE_HPP

#include <memory>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <random>
#include <cstring>
#include <tuple>

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
    short num_more_throws;   // Number of throws left for the current player
    short dice_roll;        // All the dice throws added up from when the turn began
    short last_move_id;     // Move id of the last move taken
    int* player_pos_pawn;   // For each player, for each position, the pawns that are present
    short num_blocks;       // Number of blocks present in the tracks
    Block blocks[16];       // blocks present in the tracks
    std::shared_ptr<State> previous_state;  // Memory to hold pawn and their previous position before they were moved. 
    // (This is kept to return the pawns in their original positions when there are 3 dice 6's)

    State(short n_players);
    ~State();
    
    std::shared_ptr<State> copy();    // Copies the state
    void save();        // Saves the current pawns states in 'previous_state'
    void revert();      // Reverts to the previous state
    std::string repr();
};

using StatePtr = std::shared_ptr<State>;

class Move {
public:
    int pawn;       // Pawn that is being moved
    short current_pos;     // Current position of the pawn
    short destination;          // Destination of the pawn

    Move() : pawn(0), current_pos(0), destination(0) {} // For pass move
    Move(int pawn, short current_pos, short destination) : pawn(pawn), current_pos(current_pos), destination(destination) {};

    std::string repr();
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

class NextPossiblePawns {
public:
    int* pawns;
    short* current_pos;
    short n;

    NextPossiblePawns();
    ~NextPossiblePawns();

    // Adds a pawn and it's position
    void add_pawn_pos(int pawn, short pos);

    std::string repr();
};

class LudoModel {
private:
    short* stars;   // Array that defines the positions of stars
    short* final_pos;   // Array that defines the positions of home for each colour
    short* colour_tracks;   // Sequence of positions (tracks) which pawns follow for each colour 
    short* colour_bases;    // Base positions for each colour
    std::shared_ptr<GameConfig> config;    // Config object of the game

    // Moves a single pawn from current_pos to destination in the state provided
    void move_single_pawn(StatePtr state, short player, int pawn, short current_pos, short destination);

    // Checks whether a pawn exists in an aggregate
    bool find_pawn_in_aggregate(int pawns, int pawn);   

    // Checks whether two aggregates contain the same pawns
    bool check_pawns_same(int pawns1, int pawns2); 

    // Replaces a pawn with another in an aggregate and returns the new aggregate. 
    // Note: If pawn_to_replace is not present in pawns, aggregate is not changed
    int replace_pawn_in_aggregate(int pawns, int pawn_to_replace, int pawn_to_be_replaced_with);

    // Adds a block to state
    void add_block(StatePtr state, int pawns, short pos, bool rigid);

    // Removes a block from an index
    void remove_block(StatePtr state, short index);

    // Finds whether a block is present at a pos for a given player
    bool find_block_in_position(StatePtr state, short player, short pos);

    // Get all possible pawns that can be moved forward at this state
    std::shared_ptr<NextPossiblePawns> find_next_possible_pawns(StatePtr state);

    // Given a pawn, it's current position and a dice throw, returns whether the pawn can be moved forward and to what destination
    // Note: It doesn't take into consideration the dice_roll in the state. It considers throw_ to make the function more reusable.
    std::tuple<bool, short> validate_pawn_move(StatePtr state, short throw_, short current_pos, int pawn);

    // It returns a copy of the next (board) state given a throw_, a pawn (aggregate) and it's current pos
    // Note: It doesn't take into consideration the dice_roll. It considers throw_ to make the function more reusable
    // Note2: It doesn't generate the next_throw_ in the next_state. It doesn't generate the next player. It just moves a pawn to a destination and handles capturing, blocking and so on.
    StatePtr move_pawn(StatePtr state, short throw_, short current_pos, int pawn);

    // Checks whether a heterogeneous block is present at the top of the home stretch from which a player cannot take any move
    bool check_block_no_moves_player(StatePtr state, short player);

    // Checks whether any moves are available for a given player
    bool check_available_moves(StatePtr state, short player);

public:
    std::shared_ptr<ThrowGenerator> throw_gen;

    LudoModel(std::shared_ptr<GameConfig> config);
    ~LudoModel();

    std::vector<StatePtr> generate_next_states(StatePtr state, Move move);   // Returns all next states given current state and move
    std::vector<Move> all_possible_moves(StatePtr state);    // Returns all possible moves for a given state
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
    }

    void reset();

    ~Ludo () {};

};

#endif