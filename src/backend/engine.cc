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
    if (this->player_colours)
        delete[] this->player_colours;
}

// ========================= State methods ====================================

State::State(short n_players) {
    this->n_players = n_players;
    game_over = false;
    current_player = 0;
    num_more_throws = 0;
    dice_roll = 0;
    last_move_id = 0;
    player_pos_pawn = new int[n_players * 93]();
    num_blocks = 0;
    // Note: 'blocks' will be initialized automatically
    previous_state = nullptr;
}

StatePtr State::copy() {
    StatePtr new_state = std::make_shared<State>(n_players);
    new_state->game_over = this->game_over;
    new_state->current_player = this->current_player;
    new_state->num_more_throws = this->num_more_throws;
    new_state->dice_roll = this->dice_roll;
    new_state->last_move_id = this->last_move_id;
    std::memcpy(new_state->player_pos_pawn, this->player_pos_pawn, n_players * 93 * sizeof(int));
    new_state->num_blocks = this->num_blocks;
    for (short block_idx = 0; block_idx < new_state->num_blocks; block_idx++)
        new_state->blocks[block_idx] = this->blocks[block_idx];
    if (this->previous_state)
        new_state->previous_state = this->previous_state;
    return new_state;
}

std::string State::repr() {
    
    // Creating map of names
    std::string pawn_name[] = {"", "R1", "R2", "R3", "R4", "G1", "G2", "G3", "G4", "Y1", "Y2", "Y3", "Y4", "B1", "B2", "B3",
                     "B4"};
    std::vector<std::string> pos_name = {"", "RB1", "RB2", "RB3", "RB4", "GB1", "GB2", "GB3", "GB4", "YB1", "YB2", "YB3", "YB4", "BB1",
                    "BB2", "BB3", "BB4"};
    for (int i = 0; i < 52; i++)
        pos_name.push_back("P"+std::to_string(i+1));
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
    ss << "\tnum_more_throws : " << num_more_throws << ", \n";
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


    ss << "\tprevious_state :  {\n";
    if (this->previous_state) {
        for (short player = 0; player < n_players; player++) {
            ss << "\t\tPlayer " << player << " : {\n\t\t\t";

            for (short pos = 1; pos < 93; pos++) {
                int p = this->previous_state->player_pos_pawn[player * 93 + pos];
                while (p != 0) {
                    ss << pawn_name[p % 17] << " : " << pos_name[pos] << ", ";
                    p /= 17;
                }
            }
            ss << "\n\t\t}, \n";
        }
        ss << "\t\tblocks: [\n";
        for (short block_idx = 0; block_idx < this->previous_state->num_blocks; block_idx++) {
            
            ss << "\t\t\t{ ";
            ss << "pawns : [ ";
            short p = this->previous_state->blocks[block_idx].pawns;
            while (p != 0) {
                ss << pawn_name[p% 17] << ", ";
                p /= 17;
            }
            ss << "], ";
            ss << "pos : " << pos_name[this->previous_state->blocks[block_idx].pos] << ", rigid: " << std::boolalpha << this->previous_state->blocks[block_idx].rigid;
            ss << " },\n";
        }
        ss << "\t\t], \n";

    }

    ss << "\t}\n";

    ss << "}\n";
    return ss.str();
}

void State::save() {
    this->previous_state = std::make_shared<State>(this->n_players); 
    std::memcpy(this->previous_state->player_pos_pawn, this->player_pos_pawn, this->n_players * 93 * sizeof(int));
    this->previous_state->num_blocks = this->num_blocks;
    for (short bi = 0; bi < this->num_blocks; bi++)
        this->previous_state->blocks[bi] = this->blocks[bi];
}

void State::revert() {
    if(this->previous_state) {
        std::memcpy(this->player_pos_pawn, this->previous_state->player_pos_pawn, this->n_players * 93 * sizeof(int));
        this->num_blocks = this->previous_state->num_blocks;
        for (short bi = 0; bi < this->previous_state->num_blocks; bi++)
            this->blocks[bi] = this->previous_state->blocks[bi];
    }
}

State::~State() {
    delete[] player_pos_pawn;
}

std::string Move::repr() {

    // Creating map of names
    std::string pawn_name[] = {"", "R1", "R2", "R3", "R4", "G1", "G2", "G3", "G4", "Y1", "Y2", "Y3", "Y4", "B1", "B2", "B3",
                     "B4"};
    std::vector<std::string> pos_name = {"", "RB1", "RB2", "RB3", "RB4", "GB1", "GB2", "GB3", "GB4", "YB1", "YB2", "YB3", "YB4", "BB1",
                    "BB2", "BB3", "BB4"};
    for (int i = 0; i < 52; i++)
        pos_name.push_back("P"+std::to_string(i+1));
    std::vector<std::string> last_list = {
        "RH1", "RH2", "RH3", "RH4", "RH5", "RH6", "GH1", "GH2", "GH3", "GH4", "GH5", "GH6",
        "YH1", "YH2", "YH3", "YH4", "YH5", "YH6", "BH1", "BH2", "BH3", "BH4", "BH5", "BH6"
    };
    pos_name.insert(pos_name.end(), last_list.begin(), last_list.end());
    
    std::stringstream ss;

    ss << "[ ";
    ss << "pawns : [ ";
    short p = this->pawn;
    while (p != 0) {
        ss << pawn_name[p% 17] << ", ";
        p /= 17;
    }
    ss << "], ";
    ss << "from : " << pos_name[this->current_pos] << ", to: " << pos_name[this->destination];
    ss << " ]";
    return ss.str();
}

// ========================= LudoModel methods ====================================

NextPossiblePawns::NextPossiblePawns() {
    n = 0;
    pawns = new int[93 * 16]();
    current_pos = new short[93 * 16]();
}

void NextPossiblePawns::add_pawn_pos(int pawn, short pos) {
    this->pawns[this->n] = pawn;
    this->current_pos[this->n] = pos;
    this->n++; 
}

std::string NextPossiblePawns::repr() {
    std::stringstream ss;

    ss << "NextPossiblePawns: [\n";
    for (int i = 0; i< this->n; i++) {
        ss << "\t{ " << "pawn : " << this->pawns[i] << ", pos : " << this->current_pos[i] << " },\n";
    }
    ss << "]\n";

    return ss.str();
}

NextPossiblePawns::~NextPossiblePawns() {
    delete[] pawns;
    delete[] current_pos;
}

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

    int throw_ = this->throw_gen->get_throw();
    state->dice_roll = throw_;
    
    state->num_more_throws = 0;
    if (throw_ == 6)
        state->num_more_throws++;
    
    state->last_move_id = 0;

    for (short pawn = 1; pawn < 17; pawn++) {
        short colour = (pawn - 1) / 4 + 1;
        short player = config->colour_player[colour];
        short pos = pawn;
        state->player_pos_pawn[player * 93 + pos] = pawn;
    }

    state->num_blocks = 0;
    state->save();
    return state;
}

void LudoModel::move_single_pawn(StatePtr state, short player, int pawn, short current_pos, short destination) {
    int new_pawns = 0;
    int p = state->player_pos_pawn[player * 93 + current_pos];
    while (p != 0) {
        if (p % 17 != pawn)
            new_pawns = new_pawns * 17 + (p % 17);
        p /= 17;
    }
    state->player_pos_pawn[player * 93 + current_pos] = new_pawns;
    state->player_pos_pawn[player * 93 + destination] = state->player_pos_pawn[player * 93 + destination] * 17 + pawn;
}

bool LudoModel::find_pawn_in_aggregate(int pawns, int pawn) {
    while (pawns != 0) {
        if (pawns % 17 == pawn)
            return true;
        pawns /= 17;
    }
    return false;
}

bool LudoModel::check_pawns_same(int pawns1, int pawns2) {
    while (pawns1 != 0) {
        if(!this->find_pawn_in_aggregate(pawns2, pawns1 % 17))
            return false;
        pawns1 /= 17;
    }
    return true;
}

int LudoModel::replace_pawn_in_aggregate(int pawns, int pawn_to_replace, int pawn_to_be_replace_with) {
    int p = 0;
    while (pawns != 0) {
        if (pawns % 17 == pawn_to_replace) {
            pawns = (pawns / 17) * 17 + pawn_to_be_replace_with;
            break;
        }
        else {
            p = p * 17 + pawns % 17;
        }
        pawns /= 17;
    }
    while (p != 0) {
        pawns = pawns * 17 + p % 17;
        p /= 17;
    }
    return pawns;
}

void LudoModel::add_block(StatePtr state, int pawns, short pos, bool rigid) {
    state->blocks[state->num_blocks] = Block(pawns, pos, rigid);
    state->num_blocks++;
}

void LudoModel::remove_block(StatePtr state, short index) {
    state->num_blocks--;
    state->blocks[index] = state->blocks[state->num_blocks];
}

bool LudoModel::find_block_in_position(StatePtr state, short player, short pos) {
    for (short block_idx = 0; block_idx < state->num_blocks; block_idx++) {
        if (state->blocks[block_idx].pos == pos && this->find_pawn_in_aggregate(state->player_pos_pawn[player * 93 + pos], state->blocks[block_idx].pawns % 17))
            return true;
    }
    return false;
}

std::shared_ptr<NextPossiblePawns> LudoModel::find_next_possible_pawns(StatePtr state) {
    short current_player = state->current_player;
    std::shared_ptr<NextPossiblePawns> possible_pawns = std::make_shared<NextPossiblePawns>();

    // Single pawn forward
    for (short pos = 0; pos < 93; pos++) {
        if (this->final_pos[pos] == 0) {
            int pawns = state->player_pos_pawn[current_player * 93 + pos];
            while (pawns != 0) {
                // Check whether pawn is in a block at pos
                bool block_found = false;
                for (short block_idx = 0; block_idx < state->num_blocks; block_idx++) {
                    if (state->blocks[block_idx].pos == pos && this->find_pawn_in_aggregate(state->blocks[block_idx].pawns, pawns % 17)) {
                        block_found = true;
                        break;
                    }
                }
                // If pawn is single, add it
                if (!block_found) 
                    possible_pawns->add_pawn_pos(pawns % 17, pos);
                pawns /= 17;
            }
        }
    }

    // Single pawn forward with block
    for (short pos = 0; pos < 93; pos++) {
        int p1 = state->player_pos_pawn[current_player * 93 + pos];
        while (p1 > 16) {
            int p2 = p1 / 17;
            while (p2 != 0) {

                // For each pair of pawns at pos, if both of them are present in a block, do not add them, otherwise they can be blocked up at next position
                bool p1_addable = true;
                bool p2_addable = true;
                for (short block_idx = 0; block_idx < state->num_blocks; block_idx++) {
                    if (state->blocks[block_idx].pos == pos) {
                        p1_addable = p1_addable && !this->find_pawn_in_aggregate(state->blocks[block_idx].pawns, p1 % 17);
                        p2_addable = p2_addable && (!this->find_pawn_in_aggregate(state->blocks[block_idx].pawns, p2 % 17) || !state->blocks[block_idx].rigid);
                    }
                }
                // If two single pawns are present, they can be blocked at next position
                if (p1_addable && p2_addable) 
                    possible_pawns->add_pawn_pos((p1 % 17) * 17 + p2 % 17, pos);
                p2 /= 17;
            }
            p1 /= 17;
        }
    }

    // Block pawn forward
    for (short block_idx = 0; block_idx < state->num_blocks; block_idx++) {
        if (this->find_pawn_in_aggregate(state->player_pos_pawn[current_player * 93 + state->blocks[block_idx].pos], state->blocks[block_idx].pawns % 17))
            possible_pawns->add_pawn_pos(state->blocks[block_idx].pawns, state->blocks[block_idx].pos);
    }

    // Block pawn forward unblocked after star or unrigid block
    for (short block_idx = 0; block_idx < state->num_blocks; block_idx++) {
        if ((this->stars[state->blocks[block_idx].pos] > 0 || !state->blocks[block_idx].rigid) && this->find_pawn_in_aggregate(state->player_pos_pawn[current_player * 93 + state->blocks[block_idx].pos], state->blocks[block_idx].pawns % 17)) {
            int pawns = state->blocks[block_idx].pawns;
            while (pawns != 0) {
                possible_pawns->add_pawn_pos(pawns % 17, state->blocks[block_idx].pos);
                pawns /= 17;
            }
        }
    }
    return possible_pawns;
}

std::tuple<bool, short> LudoModel::validate_pawn_move(StatePtr state, short throw_, short current_pos, int pawn) {
    short current_player = state->current_player;
    short destination = 0; // To be filled later

    // TO VERFIY
    // Single pawns:
    if (pawn <= 16) {
        short colour = (pawn - 1) / 4 + 1;
        // Pawns go to it's track only on 6 throw
        if (current_pos == pawn && throw_ != 6)
            return std::make_tuple(false, 0);
        if (current_pos == pawn && throw_ == 6)
            return std::make_tuple(true, this->colour_tracks[colour * 57 + 0]);
        // Pawns cannot jump beyond its track
        short index = 0;
        for (short track_index = 0; track_index < 57; track_index++) {
            if (this->colour_tracks[colour * 57 + track_index] == current_pos) {
                index = track_index;
                if (index + throw_ >= 57)
                    return std::make_tuple(false, 0);
            }
        }
        // Pawns cannot jump over other player's pawn blocks except pos is a base star
        for (short track_index = index + 1; track_index < index + throw_; track_index++) {
            short pos = this->colour_tracks[colour * 57 + track_index];
            if (this->stars[pos] < 2)
                for (short player = 0; player < state->n_players; player++)
                    if (player != current_player && state->player_pos_pawn[player * 93 + pos] > 16)
                        return std::make_tuple(false, 0);
        }
        // Pawns cannot move to a destination if the same player's one block and one single pawn is present except base star and final position
        destination = this->colour_tracks[colour * 57 + index + throw_];
        if (this->stars[destination] < 2 && this->final_pos[destination] == 0) {
            int pawns = state->player_pos_pawn[current_player * 93 + destination];
            short pawn_count = 0;
            while (pawns != 0) {
                pawn_count++;
                pawns /= 17;
            }
            if (pawn_count >= 3)
                return std::make_tuple(false, 0);
        } 
    }
    // Block Pawns
    else {
        // For block pawns, Odd throw_ is not valid
        if (throw_ % 2 != 0)
            return std::make_tuple(false, 0);
        // Block pawns cannot jump beyond their track
        int pawn1 = pawn % 17;
        short pawn1_colour = (pawn1 - 1) / 4 + 1;
        int pawn2 = pawn / 17;
        short pawn2_colour = (pawn2 - 1) / 4 + 1;
        short pawn1_index = 0;
        short pawn2_index = 0;
        for (short track_index = 0; track_index < 57; track_index++) {
            if (this->colour_tracks[pawn1_colour * 57 + track_index] == current_pos) 
                pawn1_index = track_index;
            if (this->colour_tracks[pawn2_colour * 57 + track_index] == current_pos)
                pawn2_index = track_index;
        }
        if (pawn1_index + throw_ / 2 >= 57 || pawn2_index + throw_ / 2 >= 57)
            return std::make_tuple(false, 0);
        // Move is possible only if both Block pawns land at the same place after moving throw_ steps
        if (this->colour_tracks[pawn1_colour * 57 + pawn1_index + throw_ / 2] != this->colour_tracks[pawn2_colour * 57 + pawn2_index + throw_ / 2])
            return std::make_tuple(false, 0);
        // Block pawns cannot jump over other pawn blocks except when pos is a base star
        for (short track_index = pawn1_index + 1; track_index < pawn1_index + throw_ / 2; track_index++) {
            short pos = this->colour_tracks[pawn1_colour * 57 + track_index];
            if (this->stars[pos] < 2)
                for (short player = 0; player < state->n_players; player++)
                    if (player != current_player && state->player_pos_pawn[player * 93 + pos] > 16)
                        return std::make_tuple(false, 0);
        }
        // Block pawns cannot move to a destination if the same player's another block is present except final position
        destination = this->colour_tracks[pawn1_colour * 57 + pawn1_index + throw_ / 2];
        if (this->final_pos[destination] == 0)
            if (state->player_pos_pawn[current_player * 93 + destination] > 16)
                return std::make_tuple(false, 0);
    }
    return std::make_tuple(true, destination);
}

StatePtr LudoModel::move_pawn(StatePtr state, short throw_, short current_pos, int pawn) {
    state = state->copy();
    short current_player = state->current_player;
    
    // If single pawn, find next position and update it
    if (pawn <= 16) {
        short colour = (pawn - 1) / 4 + 1;

        // Finding track index of the pawn's position
        short index = -6;
        for (short track_idx = 0; track_idx < 57; track_idx++)
            if (this->colour_tracks[colour * 57 + track_idx] == current_pos)
                index = track_idx;
        short destination = this->colour_tracks[colour * 57 + index + throw_];

        this->move_single_pawn(state, state->current_player, pawn, current_pos, destination);

        // If pawn is in a block, dissolve the block and leave the other pawn in old position
        for (short block_idx = 0; block_idx < state->num_blocks; block_idx++) 
            if (this->find_pawn_in_aggregate(state->blocks[block_idx].pawns, pawn)) {
                this->remove_block(state, block_idx);
                break;
            }
        
        // If at current positions two pawns are present and current position is not base star, block them up (non-rigid)
        bool block_at_current_pos = this->find_block_in_position(state, current_player, current_pos);
        if (state->player_pos_pawn[current_player * 93 + current_pos] > 16 && this->stars[current_pos] != 2 && !block_at_current_pos) {
            int new_pawns = 0;
            int p = state->player_pos_pawn[current_player * 93 + current_pos];
            short i = 0;
            while (i < 2) {
                new_pawns = new_pawns * 17 + (p % 17);
                p /= 17;
                i++;
            }
            this->add_block(state, new_pawns, current_pos, false);
        }

        // If another single pawn of other player is present at destination (except stars), capture it by sending it back to its base
        if (this->stars[destination] == 0) {
            for (short player = 0; player < state->n_players; player++) 
                if (player != current_player) {
                    int pawn_to_capture = 0;
                    int p = state->player_pos_pawn[player * 93 + destination];
                    while (p != 0) {
                        // Check whether other player's pawn is in a block. If not in block, capture it.
                        bool in_block = false;
                        for (short block_idx = 0; block_idx < state->num_blocks; block_idx++) {
                            if (state->blocks[block_idx].pos == destination && this->find_pawn_in_aggregate(state->blocks[block_idx].pawns, p % 17)) {
                                in_block = true;
                                break;
                            }
                        }
                        if (!in_block) {
                            pawn_to_capture = p % 17;
                            break;  // Breaking, as one pawn can capture only one other pawn 
                        }
                        p /= 17;
                    }
                    if (pawn_to_capture != 0) {
                        this->move_single_pawn(state, player, pawn_to_capture, destination, pawn_to_capture);
                        state->num_more_throws += 1;
                        break;  // Breaking, as capturing is performed only on one player
                    }
                }
        }

        // If another single pawn of current_player is present at destination, block it with other pawn by default except the base star positions and finale position
        bool block_at_destination = this->find_block_in_position(state, current_player, destination);
        if (this->stars[destination] < 2 && this->final_pos[destination] == 0 && state->player_pos_pawn[current_player * 93 + destination] > 16 && !block_at_destination) 
            this->add_block(state, state->player_pos_pawn[current_player * 93 + destination], destination, false);

        // If destination is finale and not all other pawns in finale position, give another move
        if (this->final_pos[destination] == 1)
            for (short pos = 0; pos < 93; pos++)
                if (pos != destination && state->player_pos_pawn[current_player * 93 + pos] > 0) {
                    state->num_more_throws += 1;
                    break;
                }
    }
    else {
        short colour = (pawn % 17 - 1) / 4 + 1;
        short index = -6;
        for (short track_idx = 0; track_idx < 57; track_idx++)
            if(this->colour_tracks[colour * 57 + track_idx] == current_pos) {
                index = track_idx;
                break;
            }
        short destination = this->colour_tracks[colour * 57 + index + throw_ / 2];

        // If moving out of a base star, check whether the block is present or not. Make a block if absent.
        short block_idx;
        if (this->stars[current_pos] == 2) {
            this->add_block(state, pawn, current_pos, false);
            block_idx = state->num_blocks - 1;
        } 
        else {
            block_idx = -1;

            // Find the block which has to be moved
            for (short bi = 0; bi < state->num_blocks; bi++)
                if (this->check_pawns_same(state->blocks[bi].pawns, pawn)) {
                    block_idx = bi;
                    break;
                }
            
            // If pawn doesn't match with any block, it means two different pawns are blocked (non-rigidly) up and one of 'pawn' is a single pawn.
            // So put the single pawn in the block and remove the other pawn from the block. 
            if (block_idx == -1) {
                int pawn_to_replace = 0;
                int pawn_to_be_kept = 0;

                // Find the pawn that needs to be kept in the block
                int p = pawn;
                while (p != 0) {
                    for (short bi = 0; bi < state->num_blocks; bi++)
                        if (this->find_pawn_in_aggregate(state->blocks[bi].pawns, p % 17)) {
                            block_idx = bi;
                            pawn_to_be_kept = p % 17;
                            break;
                        }
                    p /= 17;
                }
                
                // Find the new pawn that needs to be put in the block
                p = pawn;
                int pawn_to_be_replaced_with = 0;
                while (p != 0) {
                    if (!this->find_pawn_in_aggregate(state->blocks[block_idx].pawns, p % 17)) {
                        pawn_to_be_replaced_with = p % 17;
                        break;
                    }
                    p /= 17;
                }
                
                // Find the pawn that needs to be moved out of the block as single pawn
                p = state->blocks[block_idx].pawns;
                while (p != 0) {
                    if (p % 17 != pawn_to_be_kept) {
                        pawn_to_replace = p % 17;
                        break;
                    }
                    p /= 17;
                }
                state->blocks[block_idx].pawns = this->replace_pawn_in_aggregate(state->blocks[block_idx].pawns, pawn_to_replace, pawn_to_be_replaced_with);
            }
        }
        
        // Moving the block forward
        state->blocks[block_idx].pos = destination;
        int p = state->blocks[block_idx].pawns;
        while (p != 0) {
            this->move_single_pawn(state, state->current_player, p % 17, current_pos, destination);
            p /= 17;
        }

        // If destination is base or finale position, break the block into single pawns
        if (this->stars[destination] == 2 || this->final_pos[destination] == 1)
            this->remove_block(state, block_idx);
        // Else if destination is an intermediate star, make the block not rigid
        else if (this->stars[destination] == 1)
            state->blocks[block_idx].rigid = false;
        // Else, make the block rigid
        else
            state->blocks[block_idx].rigid = true;

        // If another block pawn of other player is present at destination position (except stars), capture them by breaking the block and sending them back to their respective bases
        if (this->stars[destination] == 0) 
            for (short player = 0; player < state->n_players; player++)
                if (player != current_player && state->player_pos_pawn[player * 93 + destination] > 16)
                    for (short bi = 0; bi < state->num_blocks; bi++)
                        if (state->blocks[bi].pos == destination && this->find_pawn_in_aggregate(state->player_pos_pawn[player * 93 + destination], state->blocks[bi].pawns % 17)) {
                            int p = state->blocks[bi].pawns;
                            this->remove_block(state, bi);
                            while (p != 0) {
                                this->move_single_pawn(state, player, p % 17, destination, p % 17);
                                p /= 17;
                            }
                            state->num_more_throws += 2;
                            break;
                        }
        
        // If destination is finale and not all other pawns in finale position, give two more throws
        if (this->final_pos[destination] == 1)
            for (short pos = 0; pos < 93; pos++)
                if (pos != destination && state->player_pos_pawn[current_player * 93 + pos] > 0) {
                    state->num_more_throws += 2;
                    break;
                }
    }
    return state;
}

bool LudoModel::check_block_no_moves_player(StatePtr state, short player) {
    short pos_to_check[] = {0, 68, 29, 42, 55};
    short colours = this->config->player_colours[player];
    // For each 'top of home stretch' position, check for heterogeneous block of player
    while (colours != 0) {
        short pos = pos_to_check[colours % 5];
        for (short block_idx = 0; block_idx < state->num_blocks; block_idx++) {
            bool pawn_of_colour_found = false;
            bool pawn_diff_colour_found = false;
            int pawns = state->blocks[block_idx].pawns;
            while (pawns != 0) {
                pawn_of_colour_found = pawn_of_colour_found || ((pawns % 17 - 1) / 4 + 1 == colours % 5);
                pawn_diff_colour_found = pawn_diff_colour_found || ((pawns % 17 - 1) / 4 + 1 != colours % 5);
                pawns /= 17;
            }
            // If a block of the particular pawn is found, it is heterogeneous and rigid in top of home stretch, then the player will have no moves
            if (state->blocks[block_idx].pos == pos && state->blocks[block_idx].rigid && pawn_of_colour_found && pawn_diff_colour_found)
                return true;
        }
        colours /= 5;
    }
    return false;
}

bool LudoModel::check_available_moves(StatePtr state, short player) {
    // If a player has a heterogeneous block at the top of home stretch, the game is immediately over for the player
    if (this->check_block_no_moves_player(state, player))
        return false;
    // If there are pawns not at the final position, then the player has moves left 
    for (short pos = 0; pos < 93; pos++) {
        if (this->final_pos[pos] != 1 && state->player_pos_pawn[player * 93 + pos] > 0)
            return true;
    }
    return false;
}

std::vector<StatePtr> LudoModel::generate_next_states(StatePtr state, Move move) {
    int throw_ = state->dice_roll;
    
    if(throw_ == 18) {
        state = state->copy();
        state->revert(); 
        state->dice_roll = 0;
    }
    else if (throw_ > 12) {
        state = this->move_pawn(state, throw_ - 12, move.current_pos, move.pawn);
        state->save();
        state->dice_roll = 0;
    }
    else if (throw_ > 6) {
        state = this->move_pawn(state, throw_ - 6, move.current_pos, move.pawn);
        if (throw_ != 12) {
            state->save();
            state->dice_roll = 0;
        }
    }
    else {
        state = this->move_pawn(state, throw_, move.current_pos, move.pawn);
        if (throw_ != 6) {
            state->save();
            state->dice_roll = 0;
        }
    }

    state->last_move_id += 1;

    if (!this->check_available_moves(state, state->current_player))
        state->num_more_throws = 0;
        
    // Change the turn
    if (state->num_more_throws == 0) {
        state->current_player = (state->current_player + 1) % state->n_players;
        state->dice_roll = 0;
        state->num_more_throws = 1;
    }
    // Check game over or not by evaluating if all other players have completed
    bool game_over = true;
    for (short player = 0; player < state->n_players; player++) 
        if (player != state->current_player && this->check_available_moves(state, player))
            game_over = false;
    state->game_over = game_over;
    state->num_more_throws--;

    std::vector<StatePtr> next_states;
    for (throw_ = 1; throw_ <= 6; throw_++) {
        StatePtr next_state = state->copy();
        next_state->dice_roll += throw_;
        if (throw_ == 6 && this->check_available_moves(next_state, next_state->current_player))
            next_state->num_more_throws++;
        next_states.push_back(next_state);
    }
    return next_states;

}

std::vector<Move> LudoModel::all_possible_moves(StatePtr state) {
    std::vector<Move> possible_moves;
    int throw_ = state->dice_roll;
    if (throw_ == 18)
        return possible_moves;  // If three 6's are rolled, no moves available
    else if (throw_ > 12)
        throw_ -= 12;
    else if (throw_ > 6)
        throw_ -= 6;

    // Generating all possible pawns that can be moved
    std::shared_ptr<NextPossiblePawns> next_possible_pawns = this->find_next_possible_pawns(state);
    
    // Validating all possible pawn moves
    for (int i = 0; i < next_possible_pawns->n ; i++) {
        int pawn = next_possible_pawns->pawns[i];
        short current_pos = next_possible_pawns->current_pos[i];
        
        std::tuple<bool, short> validation_result = this->validate_pawn_move(state, throw_, current_pos, pawn);
        
        // If pawn move is valid, put move into vector
        if (std::get<0>(validation_result))
            possible_moves.push_back(Move(pawn, current_pos, std::get<1>(validation_result)));
    }

    return possible_moves;
}

bool LudoModel::check_player_completed(StatePtr state, short player) {
    for (short pos = 0; pos < 93; pos++)
        if (this->final_pos[pos] != 1 && state->player_pos_pawn[player * 93 + pos] > 0)
            return false;
    return true;
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

void Ludo::turn(Move move, int move_id) {
    if (move_id == state->last_move_id + 1) {
        std::vector<StatePtr> next_states = this->model->generate_next_states(state, move);
        
        // Randomly selecting a next state
        int random_state_index = this->model->throw_gen->get_randint(0, next_states.size());
        this->state = next_states[random_state_index];

        // Find if any player who has completed his game. Make him the winner if the winner is not already set.
        if (this->winner == -1)
            for (short player = 0; player < this->state->n_players; player++)
                if (this->model->check_player_completed(state, player)) {
                    this->winner = player;
                    break;
                }
        
        // If the game has ended but there are no winners declared yet, set the current player as winner since the other players have no moves left
        // Note: This handles the case when both players create heterogeneous blocks that get stuck at top of home stretch
        if (this->winner == -1 && state->game_over)
            this->winner = state->current_player;
    }
}

// // Engine test
// int main() {
//     std::vector<std::vector<std::string>> colours_config = {
//         {"red", "yellow"}, 
//         {"blue", "green"}
//     };
//     Ludo game(std::make_shared<GameConfig>(colours_config));
//     game.reset();
    
//     while(!game.state->game_over) {
//         std::cout << game.state->repr();
//         std::vector<Move> moves = game.model->all_possible_moves(game.state);
//         std::cout << "Moves: [\n";
//         for (auto move : moves) {
//             std::cout << "\t" << move.repr() << std::endl;
//         }
//         std::cout << "]\n";
//         if (moves.size() > 0)
//             game.turn(moves[game.model->throw_gen->get_randint(0, moves.size())], game.state->last_move_id + 1);
//         else
//             game.turn(Move(), game.state->last_move_id + 1);
//     }
//     std::cout << game.state->repr();
//     std::cout << "Winner: " << game.winner << std::endl;

//     return 0;
// }

#endif