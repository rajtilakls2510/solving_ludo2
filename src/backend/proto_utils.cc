#ifndef LUDO_PROTO_UTILS_CC
#define LUDO_PROTO_UTILS_CC

#include "proto_utils.hpp"

ludo::BlockProto* block_to_proto(Block block) {
    ludo::BlockProto* block_proto = new ludo::BlockProto();
    block_to_proto(block, block_proto);
    return block_proto;
}

void block_to_proto(Block block, ludo::BlockProto* block_proto) {
    block_proto->set_pawns(block.pawns);
    block_proto->set_pos(block.pos);
    block_proto->set_rigid(block.rigid);
}

Block proto_to_block(ludo::BlockProto* block_proto) {
    return Block(block_proto->pawns(), block_proto->pos(), block_proto->rigid());
}

ludo::StateProto* state_to_proto(StatePtr state) {
    ludo::StateProto* state_proto = new ludo::StateProto();
    state_to_proto(state, state_proto);
    return state_proto;
}

void state_to_proto(StatePtr state, ludo::StateProto* state_proto) {
    state_proto->set_n_players(state->n_players);
    state_proto->set_game_over(state->game_over);
    state_proto->set_current_player(state->current_player);
    state_proto->set_num_more_throws(state->num_more_throws);
    state_proto->set_dice_roll(state->dice_roll);
    state_proto->set_last_move_id(state->last_move_id);
    for (short player = 0; player < state->n_players; player++)
        for (short pos = 0; pos < 93; pos++)
            state_proto->add_player_pos_pawn(state->player_pos_pawn[player * 93 + pos]);
    state_proto->set_num_blocks(state->num_blocks);
    for (short block_idx = 0; block_idx < state->num_blocks; block_idx++)
        block_to_proto(state->blocks[block_idx], state_proto->add_blocks());
    if (state->previous_state) {
        state_proto->set_has_previous(true);
        for (short player = 0; player < state->n_players; player++)
            for (short pos = 0; pos < 93; pos++)
                state_proto->add_previous_player_pos_pawn(state->previous_state->player_pos_pawn[player * 93 + pos]);
        state_proto->set_previous_num_blocks(state->previous_state->num_blocks);
        for (short block_idx = 0; block_idx < state->previous_state->num_blocks; block_idx++)
            block_to_proto(state->previous_state->blocks[block_idx], state_proto->add_previous_blocks());
    }
}

StatePtr proto_to_state(ludo::StateProto* state_proto) {
    StatePtr state = std::make_shared<State>(state_proto->n_players());
    state->game_over = state_proto->game_over();
    state->current_player = state_proto->current_player();
    state->num_more_throws = state_proto->num_more_throws();
    state->dice_roll = state_proto->dice_roll();
    state->last_move_id = state_proto->last_move_id();
    for (short player = 0; player < state->n_players; player++) 
        for (short pos = 0; pos < 93; pos++)
            state->player_pos_pawn[player * 93 + pos] = state_proto->player_pos_pawn(player * 93 + pos);
    state->num_blocks = state_proto->num_blocks();
    for (int bi = 0; bi < state->num_blocks; bi++)
        state->blocks[bi] = proto_to_block(state_proto->mutable_blocks(bi));
    if (state_proto->has_previous()) {
        state->previous_state = std::make_shared<State>(state->n_players);
        for (short player = 0; player < state->n_players; player++)
            for (short pos = 0; pos < 93; pos++)
                state->previous_state->player_pos_pawn[player * 93 + pos] = state_proto->previous_player_pos_pawn(player * 93 + pos);
        state->previous_state->num_blocks = state_proto->previous_num_blocks();
        for (short bi = 0; bi < state->previous_state->num_blocks; bi++)
            state->previous_state->blocks[bi] = proto_to_block(state_proto->mutable_previous_blocks(bi));
    }
    return state;
}

ludo::MoveProto* move_to_proto(Move move) {
    ludo::MoveProto* move_proto = new ludo::MoveProto();
    move_to_proto(move, move_proto);
    return move_proto;
}

void move_to_proto(Move move, ludo::MoveProto* move_proto) {
    move_proto->set_pawn(move.pawn);
    move_proto->set_current_pos(move.current_pos);
    move_proto->set_destination(move.destination);
}

Move proto_to_move(ludo::MoveProto* move_proto) {
    return Move(move_proto->pawn(), move_proto->current_pos(), move_proto->destination());
}

ludo::ConfigProto* config_to_proto(std::shared_ptr<GameConfig> config) {
    ludo::ConfigProto* config_proto = new ludo::ConfigProto();
    config_to_proto(config, config_proto);
    return config_proto;
}

void config_to_proto(std::shared_ptr<GameConfig> config, ludo::ConfigProto* config_proto) {
    config_proto->set_n_players(config->n_players);
    for (short colour = 0; colour < 5; colour++)
        config_proto->add_colour_player(config->colour_player[colour]);
}

std::shared_ptr<GameConfig> proto_to_config(ludo::ConfigProto* config_proto) {
    std::shared_ptr<GameConfig> config = std::make_shared<GameConfig>();
    config->n_players = config_proto->n_players();
    config->player_colours = new short[config->n_players]();
    for (short colour = 0; colour < 5; colour++) {
        config->colour_player[colour] = config_proto->colour_player(colour);
        config->player_colours[config->colour_player[colour]] = config->player_colours[config->colour_player[colour]] * 5 + colour;
    }
    return config;
}

#endif