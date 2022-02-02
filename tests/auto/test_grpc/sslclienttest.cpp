/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>
 *
 * This file is part of QtProtobuf project https://git.semlanik.org/semlanik/qtprotobuf
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and
 * to permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "testservice_grpc.qpb.h"

#include <QGrpcHttp2Channel>
#ifdef QT_PROTOBUF_NATIVE_GRPC_CHANNEL
#include <QGrpcChannel>
#endif
#include <QGrpcSslCredentials>
#include <QGrpcInsecureCallCredentials>
#include <QGrpcInsecureChannelCredentials>

#include <QTimer>
#include <QFile>
#include <QCryptographicHash>
#include <QSslConfiguration>

#include <QCoreApplication>
#include <gtest/gtest.h>

using namespace qtprotobufnamespace::tests;

class ClientTest : public ::testing::Test
{
protected:
    static void SetUpTestCase() {
        qRegisterProtobufTypes();
        qRegisterProtobufType<SimpleStringMessage>();
    }
    static QCoreApplication m_app;
    static int m_argc;
    static QUrl m_echoServerAddress;
};

int ClientTest::m_argc(0);
QCoreApplication ClientTest::m_app(m_argc, nullptr);

TEST_F(ClientTest, Http2ChannelIncorrectSecureCredentialsTest)
{
    //Prepare ssl configuration
    QSslConfiguration conf = QSslConfiguration::defaultConfiguration();
    conf.setProtocol(QSsl::TlsV1_2);
    // NOTE: CA certificate is not setup on purpose to induce the ssl handshake error
    //  QFile certFile("cert.pem");
    //  certFile.open(QIODevice::ReadOnly);
    //  QByteArray cert = certFile.readAll();
    //  conf.setCaCertificates({QSslCertificate(cert)});

    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<QGrpcHttp2Channel>(QUrl("https://localhost:60051", QUrl::StrictMode), QGrpcInsecureCallCredentials()|QGrpcSslCredentials(conf)));

    std::unique_ptr<SimpleStringMessage> result = std::make_unique<SimpleStringMessage>();
    EXPECT_FALSE(testClient.testMethod(SimpleStringMessage{"Hello Qt!"}, QPointer<SimpleStringMessage>(result.get())) == QGrpcStatus::Ok);
}

#ifdef QT_PROTOBUF_NATIVE_GRPC_CHANNEL

TEST_F(ClientTest, GrpcHttpChannelIncorrectSecureCredentialsTest)
{
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<QGrpcChannel>(QUrl("localhost:60051"), grpc::SslCredentials(grpc::SslCredentialsOptions())));

    std::unique_ptr<SimpleStringMessage> result = std::make_unique<SimpleStringMessage>();
    EXPECT_FALSE(testClient.testMethod(SimpleStringMessage{"Hello Qt!"}, QPointer<SimpleStringMessage>(result.get())) == QGrpcStatus::Ok);
}

TEST_F(ClientTest, GrpcSocketChannelIncorrectSecureCredentialsTest)
{
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<QGrpcChannel>(QUrl("unix:///tmp/test.sock"), grpc::SslCredentials(grpc::SslCredentialsOptions())));

    std::unique_ptr<SimpleStringMessage> result = std::make_unique<SimpleStringMessage>();
    EXPECT_FALSE(testClient.testMethod(SimpleStringMessage{"Hello Qt!"}, QPointer<SimpleStringMessage>(result.get())) == QGrpcStatus::Ok);
}

#endif
