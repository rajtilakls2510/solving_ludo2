#include <system.pb.h>
#include <system.grpc.pb.h>
#include <grpcpp/grpcpp.h>
#include <filesystem>
#include <fstream>
#include <thread>
#include <mutex>
namespace fs = std::filesystem;

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
        // Currently, just save the manifest. Later apply logic for old file deletion
        std::fstream output(this->games_dir / "manifest.pb", std::ios::out | std::ios::trunc | std::ios::binary);
        if (!this->manifest_proto->SerializeToOstream(&output))
            std::cerr << "Couldn't save Games manifest" << std::endl; 
    }

private:
    std::shared_ptr<alphaludo::FileNames> manifest_proto;
    std::mutex manifest_mutex;
    fs::path games_dir;
};

class Manager {
public:
    Manager(const std::string& server_addr, fs::path run_dir) {
        server_thread = std::thread([this, server_addr, run_dir] () mutable {
            
            // Starting gRPC server
            grpc::ServerBuilder builder;
            builder.AddListeningPort(server_addr, grpc::InsecureServerCredentials());
            this->games_manager = new GamesManagerService(run_dir / "games");
            builder.RegisterService(this->games_manager);

            // TODO: Register player_manager service

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
            std::this_thread::sleep_for(std::chrono::seconds(2)); // TODO:
        }
    }

private:
    std::unique_ptr<grpc::Server> server;
    std::thread server_thread;
    GamesManagerService* games_manager{nullptr};
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