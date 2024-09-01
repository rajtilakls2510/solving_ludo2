#ifndef NETWORK_UTILS_HPP
#define NETWORK_UTILS_HPP

#include <torch/torch.h>
#include "engine.hpp"
#include <iostream>

// Returns a tensor representation of the state
torch::Tensor get_state_tensor_repr(StatePtr state, std::shared_ptr<GameConfig> config);

#endif