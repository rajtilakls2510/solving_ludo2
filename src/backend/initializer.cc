#ifndef SYSTEM_INITIALIZER_CC
#define SYSTEM_INITIALIZER_CC

#include <iostream>
#include <filesystem>
#include <fstream>
#include <system.pb.h>
#include <torch/torch.h>
#include "network.hpp"
#include "network_utils.hpp"
namespace fs = std::filesystem;  


void check_and_fix_directory(fs::path agent_dir) {
    
    // Checking if agent directory is present
    if (!fs::exists(agent_dir)) {
        std::cout << agent_dir << " does not exist. Creating ..." << std::endl;
        fs::create_directories(agent_dir);
    }
    else
        std::cout << "Found " << agent_dir << std::endl;

    // Check if games directory is present
    fs::path games_dir = agent_dir / "games";
    if (!fs::exists(games_dir)) {
        std::cout << games_dir << " does not exist. Creating ..." << std::endl;
        fs::create_directories(games_dir);
    }
    else
        std::cout << "Found " << games_dir << std::endl;
    
    // Check and create manifest in games dir
    fs::path games_manifest = games_dir / "manifest.pb";
    if (!fs::exists(games_manifest)) {
        std::cout << games_manifest << " does not exist. Creating ..." << std::endl;
        alphaludo::FileNames games_manifest_proto;
        std::fstream output(games_manifest, std::ios::out | std::ios::trunc | std::ios::binary);
        if (!games_manifest_proto.SerializeToOstream(&output))
            std::cerr << "Couldn't create " << games_manifest << std::endl;
    }
    else 
        std::cout << "Found " << games_manifest << std::endl;

    // Check if players directory is present
    fs::path players_dir = agent_dir / "players";
    if (!fs::exists(players_dir)) {
        std::cout << players_dir << " does not exist. Creating ..." << std::endl;
        fs::create_directories(players_dir);
    } 
    else 
        std::cout << "Found " << players_dir << std::endl;

    // Check and create manifest is player dir
    fs::path players_manifest = players_dir / "manifest.pb";
    if (!fs::exists(players_manifest)) {
        std::cout << players_manifest << " does not exist. Creating ..." << std::endl;

        // Create an initial player and save it
        ValueNet value_net = ValueNet(26, 128, 1024, "cuda", 0.1); // Input_dim: 26, Embed_dim: 128, Value_head_internal_dim: 1024, Dropout: 0.1
        torch::optim::Adam optimizer(value_net->parameters(), torch::optim::AdamOptions(1e-4)); // Learning rate: 1e-4

        std::string checkpoint_name = get_formatted_time(std::chrono::system_clock::now()) + ".pth";
        
        torch::save(value_net, (players_dir / ("network_" + checkpoint_name)).string());
        torch::save(optimizer, (players_dir / ("optim_" + checkpoint_name)).string());

        alphaludo::FileNames players_manifest_proto;
        players_manifest_proto.add_files(checkpoint_name);
        std::fstream output(players_manifest, std::ios::out | std::ios::trunc | std::ios::binary);
        if (!players_manifest_proto.SerializeToOstream(&output))
            std::cerr << "Couldn't create " << players_manifest << std::endl;
    }
    else 
        std::cout << "Found " << players_manifest << std::endl;

}


int main(int argc, char *argv[]) {
    if (argc > 1) {
        fs::path agent_dir (argv[1]);
        check_and_fix_directory(agent_dir);
    }
    else
        std::cerr << "Arg1: path_to_run_directory" << std::endl;
    return 0;
}


#endif
