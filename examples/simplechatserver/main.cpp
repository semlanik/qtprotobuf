#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <tuple>

#include <grpc++/grpc++.h>
#include <simplechat.pb.h>
#include <simplechat.grpc.pb.h>
using namespace ::qtprotobuf::examples;

class MessageListHandler;
class UserListHandler;

class SimpleChatService final : public SimpleChat::WithAsyncMethod_messageList<SimpleChat::WithAsyncMethod_usersOnline<SimpleChat::Service>> {
public:
    Users m_usersOnline;
    Users m_usersDatabase;
    ChatMessages m_messages;

    SimpleChatService() {
        User *newUser = m_usersDatabase.add_users();
        newUser->set_name("user1");
        newUser->set_password("d8578edf8458ce06fbc5bb76a58c5ca4");
        newUser = m_usersDatabase.add_users();
        newUser->set_name("user2");
        newUser->set_password("d8578edf8458ce06fbc5bb76a58c5ca4");
        newUser = m_usersDatabase.add_users();
        newUser->set_name("user3");
        newUser->set_password("d8578edf8458ce06fbc5bb76a58c5ca4");
        newUser = m_usersDatabase.add_users();
        newUser->set_name("user4");
        newUser->set_password("d8578edf8458ce06fbc5bb76a58c5ca4");
        newUser = m_usersDatabase.add_users();
        newUser->set_name("user5");
        newUser->set_password("d8578edf8458ce06fbc5bb76a58c5ca4");
    }

    void loginUser(MessageListHandler *userHandler);

    void updateActiveUsers() {
        for(unsigned int i = 0; i < (m_activeUserListClients.size() - 1); i++) {
            m_activeUserListClients[i]->Write(m_usersOnline, nullptr);
        }
    }

    void updateMessages() {
        for(unsigned int i = 0; i < (m_activeClients.size() - 1); i++) {
            m_activeClients[i]->Write(m_messages, nullptr);
        }
    }

    bool checkUserCredentials(const std::string &name, const std::string &password) {
        for(int i = 0; i < m_usersDatabase.users_size(); i++) {
            if (m_usersDatabase.users(i).name() == name && m_usersDatabase.users(i).password() == password) {
                return true;
            }
        }

        return false;
    }

    std::tuple<std::string, std::string> extractCredentials(grpc::ServerContext *context) {
        std::string name{};
        std::string password{};
        for (auto it = context->client_metadata().begin(); it != context->client_metadata().end(); ++it) {
            if ((*it).first == std::string("user-name")) {
                name = std::string((*it).second.data());
            }
            if ((*it).first == std::string("user-password")) {
                password = std::string((*it).second.data());
            }
        }
        return std::make_tuple(name, password);
    }

    ::grpc::Status sendMessage(grpc::ServerContext *context, const ChatMessage *request, None *) override
    {
        std::string name{};
        std::string password{};
        std::tie(name, password) = extractCredentials(context);
        if (!checkUserCredentials(name, password)) {
            return ::grpc::Status(::grpc::StatusCode::UNAUTHENTICATED, "User or login are invalid");
        }

        ChatMessage *msg = m_messages.add_messages();
        *msg = *request;
        msg->set_from(name);
        updateMessages();
        return ::grpc::Status();
    }

    std::vector<::grpc::ServerAsyncWriter<ChatMessages> *> m_activeClients;
    std::vector<::grpc::ServerAsyncWriter<Users> *> m_activeUserListClients;
};

class MessageListHandler {
public:
    MessageListHandler(SimpleChatService *service, ::grpc::ServerCompletionQueue *cq) :  tag_(0xdeadbeef)
      , writer_(&ctx_)
      , cq_(cq)
    {
        service->RequestmessageList(&ctx_, &request_, &writer_, cq_, cq_, &tag_);
        service->loginUser(this);
    }
    int tag_;
    grpc::ServerContext ctx_;
    None request_;
    ::grpc::ServerAsyncWriter< ::qtprotobuf::examples::ChatMessages> writer_;
    ::grpc::ServerCompletionQueue *cq_;
};

void SimpleChatService::loginUser(MessageListHandler *handler) {
    m_activeClients.push_back(&(handler->writer_));
    //TODO: update online if required here
}

int main(int argc, char *argv[])
{
    std::string server_address("localhost:65002");
    SimpleChatService service;

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
    MessageListHandler *last = new MessageListHandler(&service, cq.get());
    while (true) {
        unsigned int *tag;
        bool ok;
        cq->Next((void**)&tag, &ok);
        if (tag == nullptr) {
            continue;
        }
        if ((*tag) == 0xdeadbeef) {
            std::string name{};
            std::string password{};
            std::tie(name, password) = service.extractCredentials(&(last->ctx_));
            if (!service.checkUserCredentials(name, password)) {
                std::cout << "Authentication failed" << std::endl;
                last->writer_.Finish(::grpc::Status(::grpc::StatusCode::UNAUTHENTICATED, "User or login are invalid"), nullptr);
                last = new MessageListHandler(&service, cq.get());
                continue;
            }
            std::cout << "Authentication ok update user chat" << std::endl;
            last->writer_.Write(service.m_messages, nullptr);
            last = new MessageListHandler(&service, cq.get());
        }
    }
}
