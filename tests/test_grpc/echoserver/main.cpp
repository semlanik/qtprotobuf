#include <iostream>
#include <grpc++/grpc++.h>
#include "testservice.pb.h"
#include "testservice.grpc.pb.h"
#include "simpletest.pb.h"
#include "simpletest.grpc.pb.h"
#include <thread>

class SimpleTestImpl final : public qtprotobufnamespace::tests::TestService::Service {
public:
    ::grpc::Status testMethod(grpc::ServerContext *, const qtprotobufnamespace::tests::SimpleStringMessage *request, qtprotobufnamespace::tests::SimpleStringMessage *response) override
    {
        std::cerr << "testMethod called" << std::endl << request->testfieldstring() << std::endl;
        response->set_testfieldstring(request->testfieldstring());
        if (request->testfieldstring() == "sleep") {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        return ::grpc::Status();
    }

    ::grpc::Status testMethodServerStream(grpc::ServerContext *, const qtprotobufnamespace::tests::SimpleStringMessage *request,
                                          ::grpc::ServerWriter<qtprotobufnamespace::tests::SimpleStringMessage> *writer) override
    {
        std::cerr << "testMethodServerStream called" << std::endl << request->testfieldstring() << std::endl;
        qtprotobufnamespace::tests::SimpleStringMessage msg;

        msg.set_testfieldstring(request->testfieldstring() + "1");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cerr << "send back " << (request->testfieldstring() + "1") << std::endl;
        writer->Write(msg);

        msg.set_testfieldstring(request->testfieldstring() + "2");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cerr << "send back " << (request->testfieldstring() + "2") << std::endl;
        writer->Write(msg);

        msg.set_testfieldstring(request->testfieldstring() + "3");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cerr << "send back " << (request->testfieldstring() + "3") << std::endl;
        writer->Write(msg);

        msg.set_testfieldstring(request->testfieldstring() + "4");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cerr << "send back " << (request->testfieldstring() + "4") << std::endl;
        writer->WriteLast(msg, grpc::WriteOptions());

        return ::grpc::Status();
    }

    ::grpc::Status testMethodBlobServerStream(grpc::ServerContext *, const qtprotobufnamespace::tests::BlobMessage *request,
                                          ::grpc::ServerWriter<qtprotobufnamespace::tests::BlobMessage> *writer) override
    {
        std::cerr << "testMethodBlobServerStream called" << std::endl;
        qtprotobufnamespace::tests::BlobMessage msg;
        msg.set_allocated_testbytes(new std::string(request->testbytes()));
        writer->Write(msg);
        return ::grpc::Status();
    }

    ::grpc::Status testMethodStatusMessage(::grpc::ServerContext*,
                                                   const ::qtprotobufnamespace::tests::SimpleStringMessage* request,
                                                   ::qtprotobufnamespace::tests::SimpleStringMessage*) override
    {
        return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, request->testfieldstring());
    }

    ::grpc::Status testMethodNonCompatibleArgRet(grpc::ServerContext *, const qtprotobufnamespace::tests::SimpleIntMessage *request, qtprotobufnamespace::tests::SimpleStringMessage *response) override
    {
        std::cerr << "testMethodNonCompatibleArgRet called" << std::endl << request->testfield() << std::endl;
        response->set_testfieldstring(std::to_string(request->testfield()));
        return ::grpc::Status();
    }

};

int main(int, char *[])
{
    std::string server_address("localhost:50051");
    std::string socket_path("unix://tmp/test.sock");
    SimpleTestImpl service;

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.AddListeningPort(socket_path, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}
