#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include "templateProto.grpc.pb.h"

class templateClient{
private:
std::unique_ptr<templateProto::Greeter::Stub> stub;
grpc::CompletionQueue cq;

struct CallData {
    templateProto::HelloRequest request;
    templateProto::HelloReply reply;
    std::unique_ptr<grpc::ClientContext> context;
    grpc::Status status;
    std::unique_ptr<grpc::ClientAsyncResponseReader<templateProto::HelloReply>> response_reader;
};
public:
    templateClient(std::shared_ptr<grpc::Channel> channel) : stub(templateProto::Greeter::NewStub(channel)) {}

   void SayHello(const std::string& user) {

        auto* call = new CallData;
        call->request.set_name(user);
        call->context = std::make_unique<grpc::ClientContext>();
        call->response_reader = stub->AsyncSayHello(call->context.get(), call->request, &cq);
        call->response_reader->Finish(&call->reply, &call->status, (void*)call);
    }
    void AsyncCompleteRpc() {
        void* got_tag;
        bool ok = false;
        while(cq.Next(&got_tag, &ok)) {
            CallData* call = static_cast<CallData*>(got_tag);
            if(call->status.ok()) {
                std::cout << "Server answered: " << call->reply.answer() << std::endl;
            } else {
                std::cout << "RPC failed" << std::endl;
            }
            delete call;
        }
    }
    int TimeWorkServer() {
        templateProto::Empty request;
        templateProto::Seconds reply;
        grpc::ClientContext context;
        grpc::Status status = stub->TimeWorkServer(&context, request, &reply);
        if(status.ok()) {
            return reply.time();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return -1;
        }
    }
};

void RunClient() {
    std::string server_address("localhost:50051");
    templateClient client(grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()));
   
    std::thread cq_thread(&templateClient::AsyncCompleteRpc, &client);
    while (true) {
        std::string text;
        std::cout << "Enter name or /STOP: ";
        std::cin >> text;
        if(text == "/STOP") {
            break;
        }
        if(text == "/TIME") {
            int time = client.TimeWorkServer();
            std::cout << "Server time: " << time << std::endl;
            continue;
        }
        client.SayHello(text);
    }
}

int main() {
    RunClient();
    return 0;
}