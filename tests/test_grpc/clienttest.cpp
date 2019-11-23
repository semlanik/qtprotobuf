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

#include "testservice_grpc.pb.h"

//#include "testserviceclient.h"
#include "qgrpchttp2channel.h"
#include "insecurecredentials.h"
//#include "blobmessage.h"
#include <sslcredentials.h>

#include <QTimer>
#include <QFile>
#include <QCryptographicHash>
#include <QSslConfiguration>

#include <QCoreApplication>
#include <gtest/gtest.h>

#include <qprotobufserializer.h>

using namespace qtprotobufnamespace::tests;
using namespace QtProtobuf;

class ClientTest : public ::testing::Test
{
protected:
    static void SetUpTestCase() {
        QtProtobuf::registerProtoTypes();
        qRegisterProtobufType<SimpleStringMessage>();
    }
    static QCoreApplication m_app;
    static int m_argc;
    static QUrl m_echoServerAddress;
};

int ClientTest::m_argc(0);
QCoreApplication ClientTest::m_app(m_argc, nullptr);
QUrl ClientTest::m_echoServerAddress("http://localhost:50051", QUrl::StrictMode);

TEST_F(ClientTest, CheckMethodsGeneration)
{
    //Dummy compile time check of functions generation and interface compatibility
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<QGrpcHttp2Channel>(QUrl(), InsecureCredentials()));
    SimpleStringMessage request;
    QPointer<SimpleStringMessage> result(new SimpleStringMessage);
    testClient.testMethod(request, result);
    testClient.testMethod(request);
    testClient.testMethod(request, &testClient, [](QGrpcAsyncReply *) {});
    delete result;
}

TEST_F(ClientTest, StringEchoTest)
{
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<QGrpcHttp2Channel>(m_echoServerAddress, InsecureCredentials()));
    SimpleStringMessage request;
    QPointer<SimpleStringMessage> result(new SimpleStringMessage);
    request.setTestFieldString("Hello beach!");
    ASSERT_TRUE(testClient.testMethod(request, result) == QGrpcStatus::Ok);
    ASSERT_STREQ(result->testFieldString().toStdString().c_str(), "Hello beach!");
    delete result;
}

TEST_F(ClientTest, StringEchoAsyncTest)
{
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<QGrpcHttp2Channel>(m_echoServerAddress, InsecureCredentials()));
    SimpleStringMessage request;
    SimpleStringMessage result;
    request.setTestFieldString("Hello beach!");
    QEventLoop waiter;

    QGrpcAsyncReply *reply = testClient.testMethod(request);
    QObject::connect(reply, &QGrpcAsyncReply::finished, &m_app, [reply, &result, &waiter]() {
        result = reply->read<SimpleStringMessage>();
        waiter.quit();
    });

    waiter.exec();
    ASSERT_STREQ(result.testFieldString().toStdString().c_str(), "Hello beach!");
}

TEST_F(ClientTest, StringEchoAsync2Test)
{
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<QGrpcHttp2Channel>(m_echoServerAddress, InsecureCredentials()));
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("Hello beach!");
    QEventLoop waiter;
    testClient.testMethod(request, &m_app, [&result, &waiter](QGrpcAsyncReply *reply) {
        result = reply->read<SimpleStringMessage>();
        waiter.quit();
    });

    waiter.exec();
    ASSERT_STREQ(result.testFieldString().toStdString().c_str(), "Hello beach!");
}

TEST_F(ClientTest, StringEchoImmediateAsyncAbortTest)
{
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<QGrpcHttp2Channel>(m_echoServerAddress, InsecureCredentials()));
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("sleep");
    QEventLoop waiter;
    QGrpcAsyncReply *reply = testClient.testMethod(request);

    result.setTestFieldString("Result not changed by echo");
    QObject::connect(reply, &QGrpcAsyncReply::finished, &m_app, [&waiter, &result, reply]() {
        result = reply->read<SimpleStringMessage>();
        waiter.quit();
    });

    QGrpcStatus::StatusCode asyncStatus = QGrpcStatus::StatusCode::Ok;
    QObject::connect(reply, &QGrpcAsyncReply::error, reply, [&asyncStatus](const QGrpcStatus &status) {
        asyncStatus = status.code();
    });

    QGrpcStatus::StatusCode clientStatus = QGrpcStatus::StatusCode::Ok;
    QObject::connect(&testClient, &TestServiceClient::error, reply, [&clientStatus](const QGrpcStatus &status) {
        clientStatus = status.code();
        std::cerr << status.code() << ":" << status.message().toStdString();
    });

    QTimer::singleShot(5000, &waiter, &QEventLoop::quit);
    reply->abort();
    waiter.exec();

    ASSERT_EQ(clientStatus, QGrpcStatus::StatusCode::Aborted);
    ASSERT_EQ(asyncStatus, QGrpcStatus::StatusCode::Aborted);
    ASSERT_STREQ(result.testFieldString().toStdString().c_str(), "Result not changed by echo");
}

TEST_F(ClientTest, StringEchoDeferredAsyncAbortTest)
{
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<QGrpcHttp2Channel>(m_echoServerAddress, InsecureCredentials()));
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("sleep");
    QEventLoop waiter;
    QGrpcAsyncReply *reply = testClient.testMethod(request);

    result.setTestFieldString("Result not changed by echo");
    bool errorCalled = false;
    reply = testClient.testMethod(request);
    QObject::connect(reply, &QGrpcAsyncReply::finished, &m_app, [reply, &result, &waiter]() {
        result = reply->read<SimpleStringMessage>();
        waiter.quit();
    });
    QObject::connect(reply, &QGrpcAsyncReply::error, reply, [&errorCalled]() {
        errorCalled = true;
    });

    QTimer::singleShot(500, reply, &QGrpcAsyncReply::abort);
    QTimer::singleShot(5000, &waiter, &QEventLoop::quit);

    waiter.exec();

    ASSERT_STREQ(result.testFieldString().toStdString().c_str(), "Result not changed by echo");
    ASSERT_TRUE(errorCalled);
}

TEST_F(ClientTest, StringEchoStreamTest)
{
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<QGrpcHttp2Channel>(m_echoServerAddress, InsecureCredentials()));
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("Stream");

    QEventLoop waiter;

    int i = 0;
    QObject::connect(&testClient, &TestServiceClient::testMethodServerStreamUpdated, &m_app, [&result, &i, &waiter](const SimpleStringMessage &ret) {
        ++i;

        result.setTestFieldString(result.testFieldString() + ret.testFieldString());

        if (i == 4) {
            waiter.quit();
        }
    });

    testClient.subscribeTestMethodServerStreamUpdates(request);

    QTimer::singleShot(20000, &waiter, &QEventLoop::quit);
    waiter.exec();

    ASSERT_EQ(i, 4);
    ASSERT_STREQ(result.testFieldString().toStdString().c_str(), "Stream1Stream2Stream3Stream4");
}

TEST_F(ClientTest, StringEchoStreamTestRetUpdates)
{
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<QGrpcHttp2Channel>(m_echoServerAddress, InsecureCredentials()));
    SimpleStringMessage request;
    QPointer<SimpleStringMessage> result(new SimpleStringMessage);

    request.setTestFieldString("Stream");

    QEventLoop waiter;

    testClient.subscribeTestMethodServerStreamUpdates(request, result);

    int i = 0;
    QObject::connect(result.data(), &SimpleStringMessage::testFieldStringChanged, &m_app, [&i]() {
        i++;
    });

    QTimer::singleShot(20000, &waiter, &QEventLoop::quit);
    waiter.exec();

    ASSERT_EQ(i, 4);
    ASSERT_STREQ(result->testFieldString().toStdString().c_str(), "Stream4");
    delete result;
}


TEST_F(ClientTest, HugeBlobEchoStreamTest)
{
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<QGrpcHttp2Channel>(m_echoServerAddress, InsecureCredentials()));
    BlobMessage result;
    BlobMessage request;
    QFile testFile("testfile");
    ASSERT_TRUE(testFile.open(QFile::ReadOnly));

    request.setTestBytes(testFile.readAll());
    QByteArray dataHash = QCryptographicHash::hash(request.testBytes(), QCryptographicHash::Sha256);
    QEventLoop waiter;

    QObject::connect(&testClient, &TestServiceClient::testMethodBlobServerStreamUpdated, &m_app, [&result, &waiter](const BlobMessage &ret) {
        result.setTestBytes(ret.testBytes());
        waiter.quit();
    });

    testClient.subscribeTestMethodBlobServerStreamUpdates(request);

    QTimer::singleShot(20000, &waiter, &QEventLoop::quit);
    waiter.exec();

    QByteArray returnDataHash = QCryptographicHash::hash(result.testBytes(), QCryptographicHash::Sha256);
    ASSERT_TRUE(returnDataHash == dataHash);
}

TEST_F(ClientTest, StatusMessageAsyncTest)
{
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<QGrpcHttp2Channel>(m_echoServerAddress, InsecureCredentials()));
    SimpleStringMessage request(QString{"Some status message"});
    QGrpcStatus::StatusCode asyncStatus = QGrpcStatus::StatusCode::Ok;
    QEventLoop waiter;
    QString statusMessage;

    QGrpcAsyncReply* reply = testClient.testMethodStatusMessage(request);
    QObject::connect(reply, &QGrpcAsyncReply::error, reply, [&asyncStatus, &waiter, &statusMessage](const QGrpcStatus &status) {
        asyncStatus = status.code();
        statusMessage = status.message();
        waiter.quit();
    });

    QTimer::singleShot(20000, &waiter, &QEventLoop::quit);
    waiter.exec();

    ASSERT_STREQ(statusMessage.toStdString().c_str(), request.testFieldString().toStdString().c_str());
}

TEST_F(ClientTest, StatusMessageClientAsyncTest)
{
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<QGrpcHttp2Channel>(m_echoServerAddress, InsecureCredentials()));
    SimpleStringMessage request(QString{"Some status message"});
    QGrpcStatus::StatusCode asyncStatus = QGrpcStatus::StatusCode::Ok;
    QEventLoop waiter;
    QString statusMessage;

    QObject::connect(&testClient, &TestServiceClient::error, [&asyncStatus, &waiter, &statusMessage](const QGrpcStatus &status) {
        asyncStatus = status.code();
        statusMessage = status.message();
        waiter.quit();
    });

    testClient.testMethodStatusMessage(request);

    QTimer::singleShot(20000, &waiter, &QEventLoop::quit);
    waiter.exec();

    ASSERT_STREQ(statusMessage.toStdString().c_str(), request.testFieldString().toStdString().c_str());
}

TEST_F(ClientTest, StatusMessageClientSyncTest)
{
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<QGrpcHttp2Channel>(m_echoServerAddress, InsecureCredentials()));
    SimpleStringMessage request(QString{"Some status message"});
    QPointer<SimpleStringMessage> ret(new SimpleStringMessage);
    QGrpcStatus::StatusCode asyncStatus = QGrpcStatus::StatusCode::Ok;
    QEventLoop waiter;
    QString statusMessage;

    QObject::connect(&testClient, &TestServiceClient::error, [&asyncStatus, &waiter, &statusMessage](const QGrpcStatus &status) {
        asyncStatus = status.code();
        statusMessage = status.message();
        waiter.quit();
    });

    testClient.testMethodStatusMessage(request, ret);
    QTimer::singleShot(20000, &waiter, &QEventLoop::quit);
    waiter.exec();

    ASSERT_STREQ(statusMessage.toStdString().c_str(), request.testFieldString().toStdString().c_str());
    delete ret;
}

TEST_F(ClientTest, StatusMessageClientSyncTestReturnedStatus)
{
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<QGrpcHttp2Channel>(m_echoServerAddress, InsecureCredentials()));
    SimpleStringMessage request(QString{"Some status message"});
    QPointer<SimpleStringMessage> ret(new SimpleStringMessage);
    QEventLoop waiter;
    QString statusMessage;

    QGrpcStatus status = testClient.testMethodStatusMessage(request, ret);

    ASSERT_STREQ(status.message().toStdString().c_str(), request.testFieldString().toStdString().c_str());
    delete ret;
}


TEST_F(ClientTest, ClientSyncTestUnattachedChannel)
{
    TestServiceClient testClient;
    SimpleStringMessage request(QString{"Some status message"});
    QPointer<SimpleStringMessage> ret(new SimpleStringMessage);
    QEventLoop waiter;

    QGrpcStatus status = testClient.testMethodStatusMessage(request, ret);

    ASSERT_EQ(status.code(), QGrpcStatus::Unknown);
    ASSERT_STREQ("No channel(s) attached.", status.message().toStdString().c_str());
    delete ret;
}

TEST_F(ClientTest, ClientSyncTestUnattachedChannelSignal)
{
    TestServiceClient testClient;
    SimpleStringMessage request(QString{"Some status message"});
    QPointer<SimpleStringMessage> ret(new SimpleStringMessage);
    QGrpcStatus asyncStatus(QGrpcStatus::StatusCode::Ok);
    QEventLoop waiter;

    QObject::connect(&testClient, &TestServiceClient::error, [&asyncStatus, &waiter](const QGrpcStatus &status) {
        asyncStatus = status;
        waiter.quit();
    });

    testClient.testMethodStatusMessage(request, ret);
    QTimer::singleShot(20000, &waiter, &QEventLoop::quit);
    waiter.exec();

    ASSERT_EQ(asyncStatus, QGrpcStatus::Unknown);
    ASSERT_STREQ("No channel(s) attached.", asyncStatus.message().toStdString().c_str());
    delete ret;
}

TEST_F(ClientTest, AsyncReplySubscribeTest)
{
    QTimer callTimeout;
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<QGrpcHttp2Channel>(m_echoServerAddress, InsecureCredentials()));
    SimpleStringMessage request(QString{"Some status message"});
    QGrpcStatus::StatusCode asyncStatus = QGrpcStatus::StatusCode::Ok;
    QEventLoop waiter;
    QString statusMessage;

    QObject::connect(&callTimeout, &QTimer::timeout, &waiter, &QEventLoop::quit);
    callTimeout.setInterval(5000);
    auto reply = testClient.testMethodStatusMessage(request);

    reply->subscribe(&m_app, []() {
        ASSERT_TRUE(false);
    },
    [&asyncStatus, &waiter, &statusMessage](const QGrpcStatus &status) {
        asyncStatus = status.code();
        statusMessage = status.message();
        waiter.quit();
    });

    callTimeout.start();
    waiter.exec();
    callTimeout.stop();

    ASSERT_STREQ(statusMessage.toStdString().c_str(), request.testFieldString().toStdString().c_str());

    SimpleStringMessage result;
    request.setTestFieldString("Hello beach!");

    reply = testClient.testMethod(request);
    reply->subscribe(&m_app, [reply, &result, &waiter]() {
        result = reply->read<SimpleStringMessage>();
        waiter.quit();
    });

    callTimeout.start();
    waiter.exec();
    callTimeout.stop();
    ASSERT_STREQ(result.testFieldString().toStdString().c_str(), request.testFieldString().toStdString().c_str());

    result.setTestFieldString("");
    request.setTestFieldString("Hello beach1!");

    reply = testClient.testMethod(request);
    reply->subscribe(&m_app, [reply, &result, &waiter]() {
        result = reply->read<SimpleStringMessage>();
        waiter.quit();
    }, []() {
        ASSERT_TRUE(false);
    });

    callTimeout.start();
    waiter.exec();
    callTimeout.stop();
    ASSERT_STREQ(result.testFieldString().toStdString().c_str(), request.testFieldString().toStdString().c_str());
}
