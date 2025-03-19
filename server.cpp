#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include "templateProto.grpc.pb.h"

class templateServer final : public templateProto::Greeter::Service {
    grpc::Status SayHello(grpc::ServerContext* context, const templateProto::HelloRequest* request, templateProto::HelloReply* response) override {
        std::cout << "Request: " << request->name() << std::endl;
        std::string prefix("Hello ");
        if(request->name() == "error") {
            return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Invalid argument");
        }
        response->set_answer(prefix + request->name());
        return grpc::Status::OK;
    }
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    templateServer service;
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "SERVER START port:" << server_address << std::endl;
    server->Wait();
}

int main(int argc, char** argv) {
    RunServer();
    return 0;
}
