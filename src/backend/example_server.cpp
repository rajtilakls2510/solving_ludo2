#include <grpcpp/grpcpp.h>
#include "service.grpc.pb.h"

class GreeterServiceImpl final : public example::Greeter::Service {
  grpc::Status SayHello(grpc::ServerContext* context, const example::HelloRequest* request, example::HelloReply* reply) override {
    std::string prefix("Hello ");
    reply->set_message(prefix + request->name());
    std::cout << "Request received from: " << request->name() << std::endl;
    return grpc::Status::OK;
  }
};

int main(int argc, char** argv) {
  std::string server_address("0.0.0.0:50051");
  GreeterServiceImpl service;

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  server->Wait();
  return 0;
}