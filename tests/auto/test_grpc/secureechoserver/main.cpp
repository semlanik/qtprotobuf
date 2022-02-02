#include <iostream>
#include <grpc++/grpc++.h>
#include <testservice.pb.h>
#include <testservice.grpc.pb.h>
#include <simpletest.pb.h>
#include <simpletest.grpc.pb.h>
#include <thread>
#include <fstream>
#include <sstream>

class SimpleTestImpl final : public qtprotobufnamespace::tests::TestService::Service {
public:
    ::grpc::Status testMethod(grpc::ServerContext *, const qtprotobufnamespace::tests::SimpleStringMessage *request, qtprotobufnamespace::tests::SimpleStringMessage *response) override
    {
        std::cerr << "testMethod called" << std::endl << request->testfieldstring() << std::endl;
        response->set_testfieldstring(request->testfieldstring());
        if (request->testfieldstring() == "sleep") {
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        return ::grpc::Status();
    }

    ::grpc::Status testMethodServerStream(grpc::ServerContext *, const qtprotobufnamespace::tests::SimpleStringMessage *request,
                                          ::grpc::ServerWriter<qtprotobufnamespace::tests::SimpleStringMessage> *writer) override
    {
        std::cerr << "testMethodServerStream called" << std::endl << request->testfieldstring() << std::endl;
        qtprotobufnamespace::tests::SimpleStringMessage msg;

        msg.set_testfieldstring(request->testfieldstring() + "1");
        std::this_thread::sleep_for(std::chrono::seconds(3));
        std::cerr << "send back " << (request->testfieldstring() + "1") << std::endl;
        writer->Write(msg);

        msg.set_testfieldstring(request->testfieldstring() + "2");
        std::this_thread::sleep_for(std::chrono::seconds(3));
        std::cerr << "send back " << (request->testfieldstring() + "2") << std::endl;
        writer->Write(msg);

        msg.set_testfieldstring(request->testfieldstring() + "3");
        std::this_thread::sleep_for(std::chrono::seconds(3));
        std::cerr << "send back " << (request->testfieldstring() + "3") << std::endl;
        writer->Write(msg);

        msg.set_testfieldstring(request->testfieldstring() + "4");
        std::this_thread::sleep_for(std::chrono::seconds(3));
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
};

int main(int, char *[])
{
    std::ifstream tfile("cert.pem");
    std::stringstream cert;
    cert << tfile.rdbuf();
    tfile.close();

    tfile.open("key.pem");
    std::stringstream key;
    key << tfile.rdbuf();
    tfile.close();

    grpc::ServerBuilder builder;
    grpc::SslServerCredentialsOptions opts(GRPC_SSL_DONT_REQUEST_CLIENT_CERTIFICATE);
    opts.pem_key_cert_pairs.push_back({key.str(), cert.str()});

    std::string server_uri("localhost:60051");
    builder.AddListeningPort(server_uri, grpc::SslServerCredentials(opts));

    std::string socket_uri("unix://tmp/test.sock");
    builder.AddListeningPort(socket_uri, grpc::SslServerCredentials(opts));

    SimpleTestImpl service;
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_uri << std::endl;

    server->Wait();
}
