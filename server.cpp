#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>

#include <grpcpp/grpcpp.h>
#include "templateProto.grpc.pb.h"

class templateServer final : public templateProto::Greeter::Service {
private:
    std::chrono::steady_clock::time_point serverStartTime;
public:
    templateServer(): serverStartTime(std::chrono::steady_clock::now()) {}

    grpc::Status SayHello(grpc::ServerContext* context, const templateProto::HelloRequest* request, templateProto::HelloReply* response) override {
        std::cout << "Request: " << request->name() << std::endl;
        
        //для наглядности работы асинхронных запросов
        std::this_thread::sleep_for(std::chrono::seconds(20));

        std::cout << "RUN"<<std::endl;
        std::string prefix("Hello ");
        if(request->name() == "error") {
            return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Invalid argument");
        }
        response->set_answer(prefix + request->name());
        return grpc::Status::OK;
    } 

    grpc::Status TimeWorkServer(grpc::ServerContext* context, const templateProto::Empty* request, templateProto::Seconds* response) override {
        std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - serverStartTime);
        response->set_time(timeSpan.count());
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

int main() {
    RunServer();
    return 0;
}
