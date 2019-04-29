#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>

#include <grpc++/grpc++.h>
#include "addressbook.pb.h"
#include "addressbook.grpc.pb.h"
using namespace ::qtprotobuf::examples;

class ContactsHandler;
class CallHandler;

class AddressBookService final : public AddressBook::WithAsyncMethod_callStatus<AddressBook::WithAsyncMethod_contacts<AddressBook::Service>> {
public:
    PhoneNumber m_lastPhone;
    CallStatus m_lastCallStatus;
    Contacts m_contacts;
    std::vector<::grpc::ServerAsyncWriter<Contacts> *> m_clients;
    std::vector<::grpc::ServerAsyncWriter<CallStatus> *> m_callClients;
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

        m_lastCallStatus.set_allocated_phonenumber(new PhoneNumber);
        m_lastCallStatus.set_status(CallStatus::Inactive);
    }

    ~AddressBookService() {}

    void updateContacts() {
        for(unsigned int i = 0; i < (m_clients.size() - 1); i++) {
            m_clients[i]->Write(m_contacts, nullptr);
        }
    }

    void registerWriter(ContactsHandler *handler);
    void registerCallStatusHandler(CallHandler *handler);

    ::grpc::Status addContact(::grpc::ServerContext *context, const Contact *request, Contacts *response) override
    {
        bool isUserOk = false;
        bool isPasswordOk = false;
        for (auto it = context->client_metadata().begin(); it != context->client_metadata().end(); ++it) {
            if ((*it).first == std::string("user-name") && (*it).second == "authorizedUser") {
                isUserOk = true;
            }
            if ((*it).first == std::string("user-password") && (*it).second == "098f6bcd4621d373cade4e832627b4f6") {
                isPasswordOk = true;
            }
            std::cout << (*it).first << (*it).second << std::endl;
        }

        if (!isUserOk || !isPasswordOk) {
            return ::grpc::Status(::grpc::StatusCode::UNAUTHENTICATED, grpc::string("Invalid user or password"));
        }

        std::cout << "addContact called" << std::endl;
        Contact* newContact = m_contacts.add_list();
        *newContact = *request;
        *response = m_contacts;
        updateContacts();
        return ::grpc::Status();
    }

    ::grpc::Status makeCall(grpc::ServerContext *, const PhoneNumber *request, CallStatus *response) override
    {
        m_lastPhone = *request;
        for(unsigned int i = 0; i < (m_callClients.size() - 1); i++) {
            response->set_status(CallStatus::Active);
            PhoneNumber *phoneNumber = new PhoneNumber(*request);
            response->set_allocated_phonenumber(phoneNumber);
            m_lastCallStatus = *response;
            m_callClients[i]->Write(m_lastCallStatus, nullptr);
        }
        return ::grpc::Status();
    }

    ::grpc::Status endCall(grpc::ServerContext *, const None *, SimpleResult *) override
    {
        if (m_lastCallStatus.status() != CallStatus::Active) {
            return ::grpc::Status();
        }

        std::cout << "Call ended" << std::endl;
        m_lastCallStatus.set_status(CallStatus::Ended);
        for(unsigned int i = 0; i < (m_callClients.size() - 1); i++) {
            m_callClients[i]->Write(m_lastCallStatus, nullptr);
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::cout << "Call ended" << std::endl;
        m_lastCallStatus.set_status(CallStatus::Inactive);
        for(unsigned int i = 0; i < (m_callClients.size() - 1); i++) {
            m_callClients[i]->Write(m_lastCallStatus, nullptr);
        }
        return ::grpc::Status();
    }

    ::grpc::Status removeContact(::grpc::ServerContext *, const Contact *, Contacts *) override
    {
        std::cout << "removeContact called" << std::endl;
        updateContacts();
        return ::grpc::Status(::grpc::UNIMPLEMENTED, "Unimplemented");
    }

    ::grpc::Status navigateTo(::grpc::ServerContext *, const Address *, SimpleResult *) override
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

class CallHandler {
public:
    CallHandler(AddressBookService* service, ::grpc::ServerCompletionQueue* cq) :  tag_(0xdeadbeee)
      , writer_(&ctx_)
      , cq_(cq)
    {
        service->RequestcallStatus(&ctx_, &request_, &writer_, cq_, cq_, &tag_);
        service->registerCallStatusHandler(this);
    }
    int tag_;
    grpc::ServerContext ctx_;
    None request_;
    ::grpc::ServerAsyncWriter< ::qtprotobuf::examples::CallStatus> writer_;
    ::grpc::ServerCompletionQueue* cq_;
};

void AddressBookService::registerWriter(ContactsHandler *handler)    {
    m_clients.push_back(&(handler->writer_));
}

void AddressBookService::registerCallStatusHandler(CallHandler *handler) {
    m_callClients.push_back(&(handler->writer_));
}

int main(int argc, char *argv[])
{
    std::string server_address("localhost:65001");
    AddressBookService service;

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
    builder.AddListeningPort(server_address, grpc::SslServerCredentials(opts));
    builder.RegisterService(&service);
    std::unique_ptr<grpc::ServerCompletionQueue> cq = builder.AddCompletionQueue();
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    ContactsHandler *last = new ContactsHandler(&service, cq.get());
    CallHandler *lastCall = new CallHandler(&service, cq.get());
    while (true) {
        unsigned int *tag;
        bool ok;
        cq->Next((void**)&tag, &ok);
        if (tag == nullptr) {
            std::cout << "Some request";
            continue;
        }
        if ((*tag) == 0xdeadbeef) {
            last->writer_.Write(service.m_contacts, nullptr);
            last = new ContactsHandler(&service, cq.get());
        }

        if ((*tag) == 0xdeadbeee) {
            lastCall->writer_.Write(service.m_lastCallStatus, nullptr);
            lastCall = new CallHandler(&service, cq.get());
        }
    }
}
