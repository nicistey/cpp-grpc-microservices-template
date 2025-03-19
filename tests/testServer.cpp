#include <gtest/gtest.h>
#include <grpcpp/grpcpp.h>
#include "templateProto.grpc.pb.h"  

class testServer final : public templateProto::Greeter::Service {
public:
    grpc::Status SayHello(grpc::ServerContext* context, const templateProto::HelloRequest* request, templateProto::HelloReply* response) override {
        response->set_answer("OK");
        return grpc::Status::OK;
    }
};

TEST(ServerTest, okReturn) {
    testServer service;
    templateProto::HelloRequest request;
    request.set_name("");
    templateProto::HelloReply reply;
    grpc::ServerContext context;
    
    grpc::Status status = service.SayHello(&context, &request, &reply);
    EXPECT_TRUE(status.ok());
    EXPECT_EQ(reply.answer(), "OK");
}