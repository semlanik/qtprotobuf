#include <iostream>
#include <grpc++/grpc++.h>
#include "addressbook.pb.h"
#include "addressbook.grpc.pb.h"

class AddressBookService final : public qtprotobuf::examples::AddressBook::Service {
public:
    AddressBookService() {}
    ~AddressBookService() {}
    ::grpc::Status addContact(::grpc::ServerContext* context, const ::qtprotobuf::examples::Contact* request, ::qtprotobuf::examples::Contacts* response) override
    {
        std::cout << "addContact called" << std::endl;
        return ::grpc::Status(::grpc::UNIMPLEMENTED, "Unimplemented");
    }
    ::grpc::Status removeContact(::grpc::ServerContext* context, const ::qtprotobuf::examples::Contact* request, ::qtprotobuf::examples::Contacts* response) override
    {
        std::cout << "removeContact called" << std::endl;
        return ::grpc::Status(::grpc::UNIMPLEMENTED, "Unimplemented");
    }
    ::grpc::Status getContacts(::grpc::ServerContext* context, const ::qtprotobuf::examples::ListFrame* request, ::qtprotobuf::examples::Contacts* response) override
    {
        std::cout << "getContacts called" << std::endl;
        ::qtprotobuf::examples::Contact* contact = response->add_list();
        contact->set_firstname("Test name 1");
        contact = response->add_list();
        contact->set_firstname("Test name 2");
        contact = response->add_list();
        contact->set_firstname("Test name 3");
        contact = response->add_list();
        contact->set_firstname("Test name 4");
        contact = response->add_list();
        contact->set_firstname("Test name 5");
        ::qtprotobuf::examples::Job *job = new ::qtprotobuf::examples::Job;
        job->set_title("Job title");
        contact->set_allocated_job(job);
        ::qtprotobuf::examples::PhoneNumber home;
        home.set_countrycode(7);
        home.set_number(1232453467);
        (*contact->mutable_phones())[::qtprotobuf::examples::Contact::Home] = home;
        return ::grpc::Status();
    }
    ::grpc::Status makeCall(::grpc::ServerContext* context, const ::qtprotobuf::examples::Contact* request, ::qtprotobuf::examples::SimpleResult* response) override
    {
        std::cout << "makeCall called" << std::endl;
        return ::grpc::Status(::grpc::UNIMPLEMENTED, "Unimplemented");
    }
    ::grpc::Status navigateTo(::grpc::ServerContext* context, const ::qtprotobuf::examples::Address* request, ::qtprotobuf::examples::SimpleResult* response) override
    {
        std::cout << "navigateTo called" << std::endl;
        return ::grpc::Status(::grpc::UNIMPLEMENTED, "Unimplemented");
    }
};

int main(int argc, char *argv[])
{
    std::string server_address("localhost:65001");
    AddressBookService service;

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}
