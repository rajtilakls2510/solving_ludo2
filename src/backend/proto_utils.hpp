#ifndef LUDO_PROTO_UTILS_HPP
#define LUDO_PROTO_UTILS_HPP

#include <ludo.pb.h>
#include "engine.hpp"

ludo::BlockProto* block_to_proto(Block block);
void block_to_proto(Block block, ludo::BlockProto* block_proto);
Block proto_to_block(ludo::BlockProto* block_proto);

ludo::StateProto* state_to_proto(StatePtr state);
void state_to_proto(StatePtr state, ludo::StateProto* state_proto);
StatePtr proto_to_state(ludo::StateProto* state_proto);

ludo::MoveProto* move_to_proto(Move move);
void move_to_proto(Move move, ludo::MoveProto* move_proto);
Move proto_to_move(ludo::MoveProto* move_proto);

ludo::ConfigProto* config_to_proto(std::shared_ptr<GameConfig> config);
void config_to_proto(std::shared_ptr<GameConfig> config, ludo::ConfigProto* config_proto);
std::shared_ptr<GameConfig> proto_to_config(ludo::ConfigProto* config_proto);

#endif