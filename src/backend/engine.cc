#ifndef LUDO_ENGINE_CPP
#define LUDO_ENGINE_CPP

#include "engine.hpp"


// ========================= GameConfig methods ====================================

GameConfig::GameConfig(std::vector<std::vector<std::string>> player_colour_choices) {
    this->n_players = player_colour_choices.size();
    this->player_colours = new short[this->n_players]();
    for (short player = 0; player < this->n_players; player++) {
        for (std::string colour: player_colour_choices[player]) {
            if (colour == "red")
                this->player_colours[player] = this->player_colours[player] * 5 + 1;
            else if (colour == "green")
                this->player_colours[player] = this->player_colours[player] * 5 + 2;
            else if (colour == "yellow")
                this->player_colours[player] = this->player_colours[player] * 5 + 3;
            else if (colour == "blue")
                this->player_colours[player] = this->player_colours[player] * 5 + 4;
            else
                throw std::runtime_error("Invalid GameConfig Provided");
        }
    }
    this->colour_player[0] = 0;
    for (short player = 0; player < this->n_players; player++) {
        std::vector<std::string> colours = player_colour_choices[player]; 
        if (std::find(colours.begin(), colours.end(), "red") != colours.end()) {
            this->colour_player[1] = player;
            break;
        }
    }
    
    for (short player = 0; player < this->n_players; player++) {
        std::vector<std::string> colours = player_colour_choices[player]; 
        if (std::find(colours.begin(), colours.end(), "green") != colours.end()) {
            this->colour_player[2] = player;
            break;
        }
    }
    
    for (short player = 0; player < this->n_players; player++) {
        std::vector<std::string> colours = player_colour_choices[player]; 
        if (std::find(colours.begin(), colours.end(), "yellow") != colours.end()) {
            this->colour_player[3] = player;
            break;
        }
    }
    
    for (short player = 0; player < this->n_players; player++) {
        std::vector<std::string> colours = player_colour_choices[player]; 
        if (std::find(colours.begin(), colours.end(), "blue") != colours.end()) {
            this->colour_player[4] = player;
            break;
        }
    }
}

std::string GameConfig::repr() {
    std::vector<std::string> colour_map = {"", "red", "green", "yellow", "blue"};

    std::stringstream ss;
    ss << "GameConfig: { \n";
    ss << "\tplayers : [ \n";
    for (short player = 0; player < this->n_players; player++) {
        ss << "\t\t{ ";
        ss << "name : Player" << player << ", colours : [ ";
        for (short c = this->player_colours[player]; c != 0; c /= 5) {
            ss << colour_map[c % 5] << ", ";
        }
        ss << "], " << " },\n";
    }
    ss << "\t],\n"; 
    ss << "}\n";

    return ss.str();
}

GameConfig::~GameConfig() {
    delete[] this->player_colours;
}

// ========================= State methods ====================================

State::State(short n_players) {
    this->n_players = n_players;
    game_over = false;
    current_player = 0;
    num_more_moves = 0;
    dice_roll = 0;
    last_move_id = 0;
    player_pos_pawn = new int[n_players * 93]();
    num_blocks = 0;
    // Note: 'blocks' will be initialized automatically
    previous_pawn_pos = 0;
}

StatePtr State::copy() {
    StatePtr new_state = std::make_shared<State>(n_players);
    new_state->game_over = this->game_over;
    new_state->current_player = this->current_player;
    new_state->num_more_moves = this->num_more_moves;
    new_state->dice_roll = this->dice_roll;
    new_state->last_move_id = this->last_move_id;
    std::memcpy(new_state->player_pos_pawn, this->player_pos_pawn, n_players * 93 * sizeof(int));
    new_state->num_blocks = this->num_blocks;
    for (short block_idx = 0; block_idx < new_state->num_blocks; block_idx++)
        new_state->blocks[block_idx] = this->blocks[block_idx];
    new_state->previous_pawn_pos = this->previous_pawn_pos;
    return new_state;
}

std::string State::repr() {
    
    // Creating map of names
    std::string pawn_name[] = {"", "R1", "R2", "R3", "R4", "G1", "G2", "G3", "G4", "Y1", "Y2", "Y3", "Y4", "B1", "B2", "B3",
                     "B4"};
    std::vector<std::string> pos_name = {"", "RB1", "RB2", "RB3", "RB4", "GB1", "GB2", "GB3", "GB4", "YB1", "YB2", "YB3", "YB4", "BB1",
                    "BB2", "BB3", "BB4"};
    for (int i = 0; i < 52; i++)
        pos_name.push_back("P"+(i+1));
    std::vector<std::string> last_list = {
        "RH1", "RH2", "RH3", "RH4", "RH5", "RH6", "GH1", "GH2", "GH3", "GH4", "GH5", "GH6",
        "YH1", "YH2", "YH3", "YH4", "YH5", "YH6", "BH1", "BH2", "BH3", "BH4", "BH5", "BH6"
    };
    pos_name.insert(pos_name.end(), last_list.begin(), last_list.end());


    // Creating printable representation
    std::stringstream ss;
    ss << "State: {\n";

    ss << "\tn_players : " << n_players << ", \n";
    ss << "\tgame_over : " << std::boolalpha << game_over << ", \n";
    ss << "\tcurrent_player : " << current_player << ", \n";
    ss << "\tnum_more_moves : " << num_more_moves << ", \n";
    ss << "\tdice_roll : " << dice_roll << ", \n";
    ss << "\tlast_move_id : " << last_move_id << ", \n";
    for (short player = 0; player < n_players; player++) {
        ss << "\tPlayer " << player << " : {\n\t\t";

        for (short pos = 1; pos < 93; pos++) {
            int p = player_pos_pawn[player * 93 + pos];
            while (p != 0) {
                ss << pawn_name[p % 17] << " : " << pos_name[pos] << ", ";
                p /= 17;
            }
        }
        ss << "\n\t}, \n";
    }
    
    // Blocks
    ss << "\tblocks: [\n";
    for (short block_idx = 0; block_idx < num_blocks; block_idx++) {
        
        ss << "\t\t{ ";
        ss << "pawns : [ ";
        short p = blocks[block_idx].pawns;
        while (p != 0) {
            ss << pawn_name[p% 17] << ", ";
            p /= 17;
        }
        ss << "], ";
        ss << "pos : " << pos_name[blocks[block_idx].pos] << ", rigid: " << std::boolalpha << blocks[block_idx].rigid;
        ss << " },\n";
    }
    ss << "\t], \n";

    int ppp = previous_pawn_pos;
    ss << "\tprevious_pawn_pos : [ \n";
    while (ppp != 0) {
        int pos = ppp % 93;
        ppp /= 93;
        int pawn = pawn % 17;
        ppp /= 17;

        ss << "\t\t{ pawn : " << pawn_name[pawn] << " , pos : " << pos_name[pos] << " },\n";   
    }
    ss << "\t]\n";


    ss << "}\n";
    return ss.str();
}

State::~State() {
    delete[] player_pos_pawn;
}

// ========================= LudoModel methods ====================================

LudoModel::LudoModel(std::shared_ptr<GameConfig> config) {
    this->config = config;
    this->throw_gen = std::make_shared<ThrowGenerator>();
    this->stars = new short[93]();
    this->final_pos = new short[93]();
    this->colour_bases = new short[5 * 4]();
    this->colour_tracks = new short[5 * 57]();

    // Setting up bases and tracks

    // RED
    for (short i = 0; i < 4; i++) 
        this->colour_bases[1 * 4 + i] = i + 1;
    for (short i = 0; i < 52; i++)
        this->colour_tracks[1 * 57 + i] = i + 18;
    for (short i = 0; i < 6; i++)
        this->colour_tracks[1 * 57 + i + 51] = i + 69;

    // GREEN
    for (short i = 0; i < 4; i++) 
        this->colour_bases[2 * 4 + i] = i + 5;
    for (short i = 0; i < 52; i++)
        if (i + 31 <= 68)
            this->colour_tracks[2 * 57 + i] = i + 31;
        else
            this->colour_tracks[2 * 57 + i] = i - 21;
    for (short i = 0; i < 6; i++)
        this->colour_tracks[2 * 57 + i + 51] = i + 75;

    // YELLOW
    for (short i = 0; i < 4; i++) 
        this->colour_bases[3 * 4 + i] = i + 9;
    for (short i = 0; i < 52; i++)
        if (i + 44 <= 68)
            this->colour_tracks[3 * 57 + i] = i + 44;
        else
            this->colour_tracks[3 * 57 + i] = i - 8;
    for (short i = 0; i < 6; i++)
        this->colour_tracks[3 * 57 + i + 51] = i + 81;

    // BLUE
    for (short i = 0; i < 4; i++) 
        this->colour_bases[4 * 4 + i] = i + 13;
    for (short i = 0; i < 52; i++)
        if (i + 57 <= 68)
            this->colour_tracks[4 * 57 + i] = i + 57;
        else
            this->colour_tracks[4 * 57 + i] = i + 5;
    for (short i = 0; i < 6; i++)
        this->colour_tracks[4 * 57 + i + 51] = i + 87;

    // Setting up stars and final positions
    
    // Normal stars
    this->stars[26] = 1;
    this->stars[39] = 1;
    this->stars[52] = 1;
    this->stars[65] = 1;
    
    // Base stars
    this->stars[18] = 2;
    this->stars[31] = 2;
    this->stars[44] = 2;
    this->stars[57] = 2;
    
    // final positions
    this->final_pos[74] = 1;
    this->final_pos[80] = 1;
    this->final_pos[86] = 1;
    this->final_pos[92] = 1;
    
}

StatePtr LudoModel::get_initial_state() {
    StatePtr state = std::make_shared<State>(config->n_players);
    state->game_over = false;
    state->current_player = 0;

    int throw_ = throw_gen->get_throw();
    state->dice_roll = throw_;
    
    state->num_more_moves = 0;
    if (throw_ == 6)
        state->num_more_moves++;
    
    state->last_move_id = 0;

    for (short pawn = 1; pawn < 17; pawn++) {
        short colour = (pawn - 1) / 4 + 1;
        short player = config->colour_player[colour];
        short pos = pawn;
        state->player_pos_pawn[player * 93 + pos] = pawn;
    }

    state->num_blocks = 0;
    state->previous_pawn_pos = 0;
    
    return state;
}

LudoModel::~LudoModel() {
    delete[] this->stars;
    delete[] this->final_pos;
    delete[] this->colour_bases;
    delete[] this->colour_tracks;
}

// ========================= Ludo methods ====================================

void Ludo::reset() {
    winner = -1;
    state = model->get_initial_state();
}


int main() {
    std::vector<std::vector<std::string>> colours_config = {
        {"red", "yellow"}, 
        {"blue", "green"}
    };
    Ludo game(std::make_shared<GameConfig>(colours_config));
    game.reset();
    std::cout << game.state->repr();
    std::cout << game.state->copy()->repr() ;
    return 0;
}

#endif