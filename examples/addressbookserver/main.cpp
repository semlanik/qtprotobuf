#include <iostream>
#include <grpc++/grpc++.h>
#include "addressbook.pb.h"
#include "addressbook.grpc.pb.h"

class AddressBookService final : public qtprotobuf::examples::AddressBook::Service {
    ::qtprotobuf::examples::Contacts m_contacts;
public:
    AddressBookService() {
        std::cout << "getContacts called" << std::endl;
        ::qtprotobuf::examples::Contact* contact = m_contacts.add_list();
        contact->set_firstname("John");
        contact->set_lastname("Doe");
        contact = m_contacts.add_list();
        contact->set_firstname("Jane");
        contact->set_lastname("Doe");
        contact = m_contacts.add_list();
        contact->set_firstname("John");
        contact->set_lastname("McClane");
        contact = m_contacts.add_list();
        contact->set_firstname("Alexey");
        contact->set_lastname("Edelev");
        contact = m_contacts.add_list();
        contact->set_firstname("Ebenezer");
        contact->set_lastname("Scrooge");
        ::qtprotobuf::examples::Job *job = new ::qtprotobuf::examples::Job;
        job->set_title("Job title");
        contact->set_allocated_job(job);
        ::qtprotobuf::examples::PhoneNumber home;
        home.set_countrycode(49);
        home.set_number(12324534679);
        (*contact->mutable_phones())[::qtprotobuf::examples::Contact::Home] = home;
    }
    ~AddressBookService() {}
    ::grpc::Status addContact(::grpc::ServerContext* context, const ::qtprotobuf::examples::Contact* request, ::qtprotobuf::examples::Contacts* response) override
    {
        std::cout << "addContact called" << std::endl;
        ::qtprotobuf::examples::Contact* newContact = m_contacts.add_list();
        *newContact = *request;
        *response = m_contacts;
        return ::grpc::Status();
    }
    ::grpc::Status removeContact(::grpc::ServerContext* context, const ::qtprotobuf::examples::Contact* request, ::qtprotobuf::examples::Contacts* response) override
    {
        std::cout << "removeContact called" << std::endl;
        return ::grpc::Status(::grpc::UNIMPLEMENTED, "Unimplemented");
    }
    ::grpc::Status getContacts(::grpc::ServerContext* context, const ::qtprotobuf::examples::ListFrame* request, ::qtprotobuf::examples::Contacts* response) override
    {
        *response = m_contacts;
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
