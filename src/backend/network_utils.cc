#ifndef NETWORK_UTILS_CC
#define NETWORK_UTILS_CC 

#include "network_utils.hpp"

torch::Tensor get_state_tensor_repr(StatePtr state, std::shared_ptr<GameConfig> config) {
    // representation[59, 26] = {R1, R2, R3, R4, G1, G2, G3, G4, Y1, Y2, Y3, Y4, B1, B2, B3, B4, RPlayer, GPlayer, YPlayer, BPlayer, currentPlayer, RPlayerRigidBlock, GPlayerRigidBlock, YPlayerRigidBlock, BPlayerRigidBlock, sum_dice_roll}
    
    torch::Tensor repr = torch::zeros({59, 26}, torch::kFloat32);
    short* pos_col_mapping = new short[17+52+6*4]();
    for (int i = 1; i < 53; i++)
        pos_col_mapping[16+i] = i;
    for (int j = 0; j < 4; j++)
        for (int i = 0; i < 6; i++)
            pos_col_mapping[17+52+j*6+i] = i+53;
    
    // Setting pawns
    for (short player = 0; player < state->n_players; player++) {
        for (int pos = 0; pos < 93; pos++) {
            int p = state->player_pos_pawn[player * 93 + pos];
            while (p != 0) {
                repr.index_put_({pos_col_mapping[pos], p % 17 - 1}, 1.0);
                p /= 17;
            }
        }
    }

    // Setting colours
    repr.index_put_({torch::indexing::Slice(), 16}, config->colour_player[1] + 1.0); // RED
    repr.index_put_({torch::indexing::Slice(), 17}, config->colour_player[2] + 1.0); // GREEN
    repr.index_put_({torch::indexing::Slice(), 18}, config->colour_player[3] + 1.0); // YELLOW
    repr.index_put_({torch::indexing::Slice(), 19}, config->colour_player[4] + 1.0); // BLUE
    repr.index_put_({torch::indexing::Slice(), 20}, state->current_player + 1.0);   // Current player

    // Setting blocks
    for (short block_idx = 0; block_idx < state->num_blocks; block_idx++) {
        short colour = (state->blocks[block_idx].pawns % 17 - 1) / 4 + 1;
        short player = config->colour_player[colour];
        for (colour = 1; colour < 5; colour++)
            if (config->colour_player[colour] == player)
                repr.index_put_({pos_col_mapping[state->blocks[block_idx].pos], 20 + colour}, state->blocks[block_idx].rigid ? 2.0 : 1.0);
    }

    // Setting dice_roll
    repr.index_put_({torch::indexing::Slice(), 25}, state->dice_roll);

    delete[] pos_col_mapping; 
    return repr;
}

std::string get_formatted_time(const std::chrono::system_clock::time_point& time_point) {
    // Convert to time_t, which holds the time in seconds
    std::time_t currentTime = std::chrono::system_clock::to_time_t(time_point);
    
    // Convert to tm struct for formatting
    std::tm* localTime = std::localtime(&currentTime);
    
    // Get milliseconds
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  time_point.time_since_epoch()) % 1000;

    // Use a stringstream for formatted output
    std::stringstream ss;
    ss << std::put_time(localTime, "%Y_%b_%d_%H_%M_%S_") << std::setw(3) << std::setfill('0') << ms.count();
    
    return ss.str();
}

#endif