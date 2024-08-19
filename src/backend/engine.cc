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


// ========================= LudoModel methods ====================================

LudoModel::LudoModel(std::shared_ptr<GameConfig> config) {
    this->config = config;
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

LudoModel::~LudoModel() {
    delete[] this->stars;
    delete[] this->final_pos;
    delete[] this->colour_bases;
    delete[] this->colour_tracks;
}


int main() {
    std::vector<std::vector<std::string>> colours_config = {
        {"red", "yellow"}, 
        {"blue", "green"}
    };
    LudoModel model(std::make_shared<GameConfig>(colours_config));
    // std::cout << config.repr();
    return 0;
}

#endif