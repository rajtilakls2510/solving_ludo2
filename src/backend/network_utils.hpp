#ifndef NETWORK_UTILS_HPP
#define NETWORK_UTILS_HPP

#include <torch/torch.h>
#include "engine.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <filesystem>
namespace fs = std::filesystem;

// Returns a tensor representation of the state
torch::Tensor get_state_tensor_repr(StatePtr state, std::shared_ptr<GameConfig> config);

std::string get_formatted_time(const std::chrono::system_clock::time_point& time_point);
#endif