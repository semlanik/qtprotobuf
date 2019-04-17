#include <QCoreApplication>

#include <iostream>
#include <grpc++/grpc++.h>
#include <testservice.pb.h>
#include <testservice.grpc.pb.h>
#include <simpletest.pb.h>
#include <simpletest.grpc.pb.h>
#include <thread>

class SimpleTestImpl final : public qtprotobufnamespace::tests::TestService::Service {
public:
    ::grpc::Status testMethod(grpc::ServerContext *, const qtprotobufnamespace::tests::SimpleStringMessage *request, qtprotobufnamespace::tests::SimpleStringMessage *response)
    {
        std::cerr << "testMethod called" << std::endl << request->testfieldstring() << std::endl;
        response->set_testfieldstring(request->testfieldstring());
        if(request->testfieldstring() == "sleep") {
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        return ::grpc::Status();
    }
};

int main(int, char *[])
{
    std::string server_address("localhost:50051");
    SimpleTestImpl service;

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}
