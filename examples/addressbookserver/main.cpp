#include <iostream>
#include <vector>
#include <grpc++/grpc++.h>
#include "addressbook.pb.h"
#include "addressbook.grpc.pb.h"
using namespace ::qtprotobuf::examples;

class ContactsHandler;

class AddressBookService final : public AddressBook::WithAsyncMethod_contacts<AddressBook::Service> {
public:
    Contacts m_contacts;
    std::vector<::grpc::ServerAsyncWriter<Contacts> *> m_clients;
    AddressBookService(): m_clients({}) {
        Contact* contact = m_contacts.add_list();
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
        Job *job = new Job;
        job->set_title("Job title");
        contact->set_allocated_job(job);
        PhoneNumber *home = (*contact->mutable_phones()).Add();
        home->set_countrycode(49);
        home->set_number(12324534679);
    }

    ~AddressBookService() {}

    void updateContacts() {
        for(int i = 0; i < (m_clients.size() - 1); i++) {
            m_clients[i]->Write(m_contacts, nullptr);
        }
    }

    void registerWriter(ContactsHandler *handler);

    ::grpc::Status addContact(::grpc::ServerContext* context, const Contact* request, Contacts* response) override
    {
        std::cout << "addContact called" << std::endl;
        Contact* newContact = m_contacts.add_list();
        *newContact = *request;
        *response = m_contacts;
        updateContacts();
        return ::grpc::Status();
    }
    ::grpc::Status removeContact(::grpc::ServerContext* context, const Contact* request, Contacts* response) override
    {
        std::cout << "removeContact called" << std::endl;
        updateContacts();
        return ::grpc::Status(::grpc::UNIMPLEMENTED, "Unimplemented");
    }
    ::grpc::Status makeCall(::grpc::ServerContext* context, const Contact* request, SimpleResult* response) override
    {
        std::cout << "makeCall called" << std::endl;
        return ::grpc::Status(::grpc::UNIMPLEMENTED, "Unimplemented");
    }
    ::grpc::Status navigateTo(::grpc::ServerContext* context, const Address* request, SimpleResult* response) override
    {
        std::cout << "navigateTo called" << std::endl;
        return ::grpc::Status(::grpc::UNIMPLEMENTED, "Unimplemented");
    }
};

class ContactsHandler {
public:
    ContactsHandler(AddressBookService* service, ::grpc::ServerCompletionQueue* cq) :  tag_(0xdeadbeef)
      , writer_(&ctx_)
      , cq_(cq)
    {
        service->Requestcontacts(&ctx_, &request_, &writer_, cq_, cq_, &tag_);
        service->registerWriter(this);
    }
    int tag_;
    grpc::ServerContext ctx_;
    ListFrame request_;
    ::grpc::ServerAsyncWriter< ::qtprotobuf::examples::Contacts> writer_;
    ::grpc::ServerCompletionQueue* cq_;
};


void AddressBookService::registerWriter(ContactsHandler *handler)    {
    m_clients.push_back(&(handler->writer_));
}

int main(int argc, char *argv[])
{
    std::string server_address("localhost:65001");
    AddressBookService service;

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::ServerCompletionQueue> cq = builder.AddCompletionQueue();
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    ContactsHandler *last = new ContactsHandler(&service, cq.get());
    while (true) {
        int *tag;
        bool ok;
        cq->Next((void**)&tag, &ok);
        if (tag == nullptr) {
            continue;
        }
        if ((*tag) == 0xdeadbeef) {
            last->writer_.Write(service.m_contacts, nullptr);
            last = new ContactsHandler(&service, cq.get());
        }
    }
}
