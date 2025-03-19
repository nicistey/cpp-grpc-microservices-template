#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include "templateProto.grpc.pb.h"

class templateClient{
private:
std::unique_ptr<templateProto::Greeter::Stub> stub;
public:
    templateClient(std::shared_ptr<grpc::Channel> channel) : stub(templateProto::Greeter::NewStub(channel)) {}

    std::string SayHello(const std::string& user) {
        templateProto::HelloRequest request;
        request.set_name(user);
        templateProto::HelloReply reply;
        grpc::ClientContext context;
        grpc::Status status = stub->SayHello(&context, request, &reply);
        if(status.ok()) {
            return reply.answer();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return "RPC failed";
        }
    }
};

void RunClient() {
    std::string server_address("localhost:50051");
    templateClient client(grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()));
    while (true) {
        std::string user;
        std::cout << "Enter name or /STOP: ";
        std::cin >> user;
        if(user == "/STOP") {
            break;
        }
        std::string reply = client.SayHello(user);
        std::cout << "Server answered: " << reply << std::endl;
    }
}

int main() {
    RunClient();
    return 0;
}