#include <system.pb.h>
#include <system.grpc.pb.h>
#include <grpcpp/grpcpp.h>
#include <filesystem>
#include <fstream>
#include <thread>
#include <mutex>
#include "engine.hpp"
#include "proto_utils.hpp"
#include <condition_variable>
#include <random>
#include <stdexcept>
namespace fs = std::filesystem;


class Event {
// Python threading.Event() equivalent class
public:
    Event() : flag(false) {}

    // Set the event
    void set() {
        std::lock_guard<std::mutex> lock(mtx);
        flag = true;
        cv.notify_all();
    }

    // Clear the event
    void clear() {
        std::lock_guard<std::mutex> lock(mtx);
        flag = false;
    }

    // Wait until the event is set
    void wait() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return flag; });
    }

    // Wait with timeout
    bool wait_for(std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(mtx);
        return cv.wait_for(lock, timeout, [this] { return flag; });
    }

private:
    std::mutex mtx;
    std::condition_variable cv;
    bool flag;
};


// ============================= Games Manager ================================


class GamesManagerService final : public alphaludo::GamesManager::Service {
public:
    GamesManagerService(fs::path games_dir) : games_dir(games_dir) {
        this->manifest_proto = std::make_shared<alphaludo::FileNames>();
        std::fstream input(games_dir / "manifest.pb", std::ios::in | std::ios::binary);
        if (!this->manifest_proto->ParseFromIstream(&input))
            std::cerr << "Couldn't parse Games manifest" << std::endl;
    }

    // Implement rpc methods
    
    virtual ::grpc::Status Save(::grpc::ServerContext* context, const ::alphaludo::FileName* request, ::google::protobuf::Empty* response) {
        if (fs::exists(games_dir / request->file())) { 
            this->manifest_mutex.lock();
            // Check if already present in manifest
            bool found = false;
            for (int i = 0; i < this->manifest_proto->files_size(); i++)
                found = found || this->manifest_proto->files(i) == request->file();
            if (!found)
                this->manifest_proto->add_files(request->file());
            this->manifest_mutex.unlock();

            return ::grpc::Status::OK;
        }
        return grpc::Status(grpc::StatusCode::NOT_FOUND, request->file() + " was not found in games directory");
    }

    virtual ::grpc::Status GetAll(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::alphaludo::FileNames* response) {
        this->manifest_mutex.lock();
        for (int i = 0; i < this->manifest_proto->files_size(); i++) {
            response->add_files(this->manifest_proto->files(i));
        }
        this->manifest_mutex.unlock();
        return ::grpc::Status::OK;
    }
    
    virtual ::grpc::Status Get(::grpc::ServerContext* context, const ::alphaludo::FileName* request, ::ludo::GameProto* response) {
        if (fs::exists(games_dir / request->file())) { // Or, should have checked in manifest too
            std::fstream input(games_dir / request->file(), std::ios::in | std::ios::binary);
            if(!response->ParseFromIstream(&input))
                return ::grpc::Status(grpc::StatusCode::UNKNOWN, "Couldn't parse "+request->file());
            return ::grpc::Status::OK;
        }

        return grpc::Status(grpc::StatusCode::NOT_FOUND, request->file() + " was not found in games directory");
    }

    void persist() {
        // TODO: Currently, just save the manifest. Later apply logic for old file deletion
        std::fstream output(this->games_dir / "manifest.pb", std::ios::out | std::ios::trunc | std::ios::binary);
        if (!this->manifest_proto->SerializeToOstream(&output))
            std::cerr << "Couldn't save players manifest" << std::endl; 
    }

private:
    std::shared_ptr<alphaludo::FileNames> manifest_proto;
    std::mutex manifest_mutex;
    fs::path games_dir;
};


// ========================= Player Manager =============================


class PlayerManagerService final : public alphaludo::PlayerManager::Service {
public:
    PlayerManagerService(fs::path players_dir) : players_dir(players_dir), gen(std::random_device{}()) {
        this->manifest_proto = std::make_shared<alphaludo::FileNames>();
        std::fstream input(players_dir / "manifest.pb", std::ios::in | std::ios::binary);
        if (!this->manifest_proto->ParseFromIstream(&input))
            std::cerr << "Couldn't parse players manifest" << std::endl;
    }

    // Implement RPC methods

    virtual ::grpc::Status Save(::grpc::ServerContext* context, const ::alphaludo::FileName* request, ::google::protobuf::Empty* response) {
        if (fs::exists(this->players_dir / ("network_" + request->file())) && fs::exists(this->players_dir / ("optim_" + request->file()))) { 
            this->manifest_mutex.lock();
            // Check if already present in manifest
            bool found = false;
            for (int i = 0; i < this->manifest_proto->files_size(); i++)
                found = found || this->manifest_proto->files(i) == request->file();
            if (!found)
                this->manifest_proto->add_files(request->file());
            this->manifest_mutex.unlock();

            return ::grpc::Status::OK;
        }
        return grpc::Status(grpc::StatusCode::NOT_FOUND, request->file() + " was not found in players directory");
    }

    virtual ::grpc::Status GetAll(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::alphaludo::FileNames* response) {
        this->manifest_mutex.lock();
        for (int i = 0; i < this->manifest_proto->files_size(); i++) {
            response->add_files(this->manifest_proto->files(i));
        }
        this->manifest_mutex.unlock();
        return ::grpc::Status::OK;
    }

    virtual ::grpc::Status GetLatest(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::alphaludo::FileName* response) {
        this->manifest_mutex.lock();
        if (this->manifest_proto->files_size() > 0) {
            response->set_file(this->manifest_proto->files(this->manifest_proto->files_size() - 1));
            this->manifest_mutex.unlock();
            return ::grpc::Status::OK;
        }
        this->manifest_mutex.unlock();
        return grpc::Status(grpc::StatusCode::UNAVAILABLE, "No player saved!");
    }
    
    virtual ::grpc::Status GetRandom(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::alphaludo::FileName* response) {
        this->manifest_mutex.lock();
        if (this->manifest_proto->files_size() > 0) {
            std::uniform_int_distribution<> distrib(0, this->manifest_proto->files_size()-1);
            int chosen_index = distrib(this->gen);
            response->set_file(this->manifest_proto->files(chosen_index));
            this->manifest_mutex.unlock();
            return ::grpc::Status::OK;
        }
        this->manifest_mutex.unlock();
        return grpc::Status(grpc::StatusCode::UNAVAILABLE, "No player saved!");
    }

    void persist() {
        // TODO: Currently, just save the manifest. Later apply logic for old file deletion
        std::fstream output(this->players_dir / "manifest.pb", std::ios::out | std::ios::trunc | std::ios::binary);
        if (!this->manifest_proto->SerializeToOstream(&output))
            std::cerr << "Couldn't save players manifest" << std::endl; 
    }
private:
    std::shared_ptr<alphaludo::FileNames> manifest_proto;
    std::mutex manifest_mutex;
    fs::path players_dir;
    std::mt19937 gen;
};


// ======================== Liveplay Manager =============================

class Agent {
public:
    Agent (int player_index, std::shared_ptr<Ludo> game_engine) : player_index(player_index), game_engine(game_engine) {}
    virtual void take_next_move() {};
    virtual std::string get_mode() {return "Agent";}
    virtual void move_taken(Move move) {}
    virtual void stop() {}
    virtual ~Agent() {};
protected:
    int player_index;
    std::shared_ptr<Ludo> game_engine;
};

class HumanAgent : public Agent {
public:
    HumanAgent(int player_index, std::shared_ptr<Ludo> game_engine) : Agent(player_index, game_engine) {};
    virtual std::string get_mode() override {
        return "Human";
    }
};

class LiveplayManagerService final : public alphaludo::LiveplayManager::Service {
public:
    LiveplayManagerService() : running{false} {
        this->move_event.set();
        this->create_event.set();
    }

    // Implement RPC methods

    virtual ::grpc::Status CheckRunningGame(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::google::protobuf::Empty* response) {
        this->create_event.wait(); // Wait if a game is being created
        if (!this->game)
            return grpc::Status(grpc::StatusCode::NOT_FOUND, "No game found");
        return grpc::Status::OK;
    }
    
    virtual ::grpc::Status Reset(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::google::protobuf::Empty* response) {
        this->create_event.wait(); // Wait if a game is being created
        this->move_event.wait();
        // Now reset
        this->game = nullptr;
        return grpc::Status::OK;
    }
    
    virtual ::grpc::Status CreateNewGame(::grpc::ServerContext* context, const ::alphaludo::NewGameConfig* request, ::alphaludo::StateResponse* response) {
        this->create_event.clear();
        this->create_mtx.lock();
        if (!this->game) {

            // Parse config
            std::vector<std::vector<std::string>> colours_config;
            for (int player = 0; player < request->player_configs_size(); player++) {
                const alphaludo::PlayerConfig& player_config = request->player_configs(player);
                std::vector<std::string> colours;
                for (int colour = 0; colour < player_config.colours_size(); colour++)
                    colours.push_back(player_config.colours(colour));
                colours_config.push_back(colours); 
            }
            
            // Create game object
            std::shared_ptr<GameConfig> game_config = std::make_shared<GameConfig>(colours_config);
            this->game = std::make_shared<Ludo>(game_config);
            this->game->reset();
            
            // Create Agents
            std::vector<Agent> agents;
            for (int player = 0; player < request->player_configs_size(); player++) {    
                // TODO:
                // std::cout << request->player_configs(player).mode() << std::endl;
                agents.push_back(HumanAgent(player, this->game));
            }
            this->players = agents;
            
        }
        // Set current state to response and all available moves
        config_to_proto(this->game->model->get_config(), response->mutable_config());
        for (Agent player: this->players) {   
            alphaludo::PlayerMode mode = player.get_mode() == "AI" ? alphaludo::AI : alphaludo::HUMAN; 
            response->add_modes(mode);
        }
        state_to_proto(this->game->state, response->mutable_state());
        for (Move move: this->game->model->all_possible_moves(this->game->state)) {
            move_to_proto(move, response->add_moves());
        }

        // Notify the current player to take turn
        std::thread t(&Agent::take_next_move, &this->players[this->game->state->current_player]);
        t.detach();
        
        this->create_event.set();
        this->create_mtx.unlock();

        return grpc::Status::OK;
    }
    
    virtual ::grpc::Status GetCurrentState(::grpc::ServerContext* context, const ::google::protobuf::Empty* request, ::alphaludo::StateResponse* response) {
        this->create_event.wait();
        this->move_event.wait();    // Wait for the move being taken
        // Set current state to response and all available moves
        config_to_proto(this->game->model->get_config(), response->mutable_config());
        for (Agent player: this->players) {   
            alphaludo::PlayerMode mode = player.get_mode() == "AI" ? alphaludo::AI : alphaludo::HUMAN; 
            response->add_modes(mode);
        }
        state_to_proto(this->game->state, response->mutable_state());
        for (Move move: this->game->model->all_possible_moves(this->game->state)) {
            move_to_proto(move, response->add_moves());
        }
        return grpc::Status::OK;
    }
    
    virtual ::grpc::Status TakeMove(::grpc::ServerContext* context, const ::alphaludo::MoveRequest* request, ::alphaludo::StateResponse* response) {
        this->create_event.wait();
        this->move_event.clear();
        this->move_mtx.lock();

        if (request->move_id() == this->game->state->last_move_id + 1) {
            
            // Extract move and take turn
            ludo::MoveProto move_proto;
            move_proto.CopyFrom(request->move()); 
            Move move = proto_to_move(&move_proto);
            this->game->turn(move, request->move_id());

            // Notify players of move taken
            for (Agent player: this->players)
                player.move_taken(move);
            
            if (this->game->state->game_over){
                // TODO: Log data and send to games manager service

            }
            else {
                // Notify current player to take turn
                std::thread t(&Agent::take_next_move, &this->players[this->game->state->current_player]);
                t.detach();
            }
        }
        // Set current state to response and all available moves
        config_to_proto(this->game->model->get_config(), response->mutable_config());
        for (Agent player: this->players) {   
            alphaludo::PlayerMode mode = player.get_mode() == "AI" ? alphaludo::AI : alphaludo::HUMAN; 
            response->add_modes(mode);
        }
        state_to_proto(this->game->state, response->mutable_state());
        for (Move move: this->game->model->all_possible_moves(this->game->state)) {
            move_to_proto(move, response->add_moves());
        }
        this->move_mtx.unlock();
        this->move_event.set();
        return grpc::Status::OK;
    }


public:
    bool running;
    std::shared_ptr<Ludo> game{nullptr};
private:
    std::vector<Agent> players;
    Event move_event, create_event;
    std::mutex move_mtx, create_mtx;
};

class Manager {
public:
    Manager(const std::string& server_addr, fs::path run_dir) {
        server_thread = std::thread([this, server_addr, run_dir] () mutable {
            
            // Starting gRPC server
            grpc::ServerBuilder builder;
            builder.AddListeningPort(server_addr, grpc::InsecureServerCredentials());
            this->games_manager = new GamesManagerService(run_dir / "games");
            this->player_manager = new PlayerManagerService(run_dir / "players");
            this->live_manager = new LiveplayManagerService();

            builder.RegisterService(this->games_manager);
            builder.RegisterService(this->player_manager);
            builder.RegisterService(this->live_manager);

            // Set the maximum receive message size 128 MB (in bytes)
            builder.SetMaxReceiveMessageSize(128 * 1024 * 1024);
            this->server = builder.BuildAndStart();
            if (this->server) {
                std::cout << "Server listening on " << server_addr << std::endl;
            } else {
                throw std::runtime_error("Failed to start server");
            }
            this->server->Wait();
        });
    }

    void manage() {
        while (true) {
            if (this->games_manager) 
                this->games_manager->persist();
            if (this->player_manager)
                this->player_manager->persist();
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    }

private:
    std::unique_ptr<grpc::Server> server;
    std::thread server_thread;
    GamesManagerService* games_manager{nullptr};
    PlayerManagerService* player_manager{nullptr};
    LiveplayManagerService* live_manager{nullptr};
};


int main(int argc, char *argv[]) {
    if (argc > 1) {
        fs::path run_dir (argv[1]);
        Manager manager ("0.0.0.0:50051", run_dir);
        manager.manage();
    }
    else 
        std::cerr << "Arg1: path_to_run_directory" << std::endl;
    return 0;
}