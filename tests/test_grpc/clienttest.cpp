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
#include <QGrpcCredentials>
#include <QGrpcInsecureCredentials>

#include <QTimer>
#include <QFile>
#include <QCryptographicHash>
#include <QThread>

#include <QCoreApplication>

#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>

#include <qprotobufserializer.h>

using namespace qtprotobufnamespace::tests;
using namespace QtProtobuf;

typedef TestServiceClient* createTestServiceClientFunc();

class ClientTest : public ::testing::TestWithParam<createTestServiceClientFunc*>
{
public:
    static TestServiceClient * createHttp2Client() {
        auto *c = new TestServiceClient();
        c->attachChannel(std::make_shared<QGrpcHttp2Channel>(ClientTest::m_echoServerAddress, QGrpcInsecureChannelCredentials() | QGrpcInsecureCallCredentials())); \
        return c;
    }

#ifdef QT_PROTOBUF_NATIVE_GRPC_CHANNEL
    static TestServiceClient * createGrpcSocketClient() {
        auto *c = new TestServiceClient();
        c->attachChannel(std::make_shared<QGrpcChannel>(ClientTest::m_echoServerSocket, grpc::InsecureChannelCredentials())); \
        return c;
    }

    static TestServiceClient * createGrpcHttpClient() {
        auto *c = new TestServiceClient();
        c->attachChannel(std::make_shared<QGrpcChannel>(ClientTest::m_echoServerAddressNative, grpc::InsecureChannelCredentials())); \
        return c;
    }
#endif

    static createTestServiceClientFunc *clientCreators[];

protected:
    static void SetUpTestCase() {
        QtProtobuf::qRegisterProtobufTypes();
    }
    static QCoreApplication m_app;
    static int m_argc;
    static const QUrl m_echoServerAddress;
    static const QString m_echoServerSocket;
    static const QString m_echoServerAddressNative;
};

int ClientTest::m_argc(0);
QCoreApplication ClientTest::m_app(m_argc, nullptr);
const QUrl ClientTest::m_echoServerAddress("http://localhost:50051", QUrl::StrictMode);
const QString ClientTest::m_echoServerAddressNative("localhost:50051");
const QString ClientTest::m_echoServerSocket("unix:///tmp/test.sock");
createTestServiceClientFunc* ClientTest::clientCreators[]{
    ClientTest::createHttp2Client,
#ifdef QT_PROTOBUF_NATIVE_GRPC_CHANNEL
    ClientTest::createGrpcHttpClient,
    ClientTest::createGrpcSocketClient,
#endif
};

TEST_F(ClientTest, CheckMethodsGeneration)
{
    //Dummy compile time check of functions generation and interface compatibility
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<QGrpcHttp2Channel>(QUrl(), QGrpcInsecureChannelCredentials() | QGrpcInsecureCallCredentials()));
    SimpleStringMessage request;
    QPointer<SimpleStringMessage> result(new SimpleStringMessage);
    testClient.testMethod(request, result);
    testClient.testMethod(request);
    testClient.testMethod(request, &testClient, [](QGrpcCallReplyShared) {});
    delete result;
}

TEST_P(ClientTest, StringEchoTest)
{
    auto testClient = (*GetParam())();
    SimpleStringMessage request;
    QPointer<SimpleStringMessage> result(new SimpleStringMessage);
    request.setTestFieldString("Hello beach!");
    ASSERT_TRUE(testClient->testMethod(request, result) == QGrpcStatus::Ok);
    ASSERT_STREQ(result->testFieldString().toStdString().c_str(), "Hello beach!");
    delete result;
    testClient->deleteLater();
}

TEST_P(ClientTest, StringEchoAsyncTest)
{
    auto testClient = (*GetParam())();
    SimpleStringMessage request;
    SimpleStringMessage result;
    request.setTestFieldString("Hello beach!");
    QEventLoop waiter;

    QGrpcCallReplyShared reply = testClient->testMethod(request);
    QObject::connect(reply.get(), &QGrpcCallReply::finished, &m_app, [reply, &result, &waiter]() {
        result = reply->read<SimpleStringMessage>();
        reply->deleteLater();
        waiter.quit();
    });

    waiter.exec();
    ASSERT_STREQ(result.testFieldString().toStdString().c_str(), "Hello beach!");
    testClient->deleteLater();
}

TEST_P(ClientTest, StringEchoAsync2Test)
{
    auto testClient = (*GetParam())();
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("Hello beach!");
    QEventLoop waiter;
    testClient->testMethod(request, &m_app, [&result, &waiter](QGrpcCallReplyShared reply) {
        result = reply->read<SimpleStringMessage>();
        waiter.quit();
    });

    waiter.exec();
    ASSERT_STREQ(result.testFieldString().toStdString().c_str(), "Hello beach!");
    testClient->deleteLater();
}


TEST_P(ClientTest, StringEchoImmediateAsyncAbortTest)
{
    auto testClient = (*GetParam())();
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("sleep");
    QEventLoop waiter;
    QGrpcCallReplyShared reply = testClient->testMethod(request);

    result.setTestFieldString("Result not changed by echo");
    QObject::connect(reply.get(), &QGrpcCallReply::finished, &m_app, [&waiter, &result, reply]() {
        result = reply->read<SimpleStringMessage>();
        reply->deleteLater();
        waiter.quit();
    });

    QGrpcStatus::StatusCode asyncStatus = QGrpcStatus::StatusCode::Ok;
    QObject::connect(reply.get(), &QGrpcCallReply::error, [&asyncStatus](const QGrpcStatus &status) {
        asyncStatus = status.code();
    });

    QGrpcStatus::StatusCode clientStatus = QGrpcStatus::StatusCode::Ok;
    QObject::connect(testClient, &TestServiceClient::error, [&clientStatus](const QGrpcStatus &status) {
        clientStatus = status.code();
        std::cerr << status.code() << ":" << status.message().toStdString();
    });

    QTimer::singleShot(5000, &waiter, &QEventLoop::quit);
    reply->abort();
    waiter.exec();

    ASSERT_EQ(clientStatus, QGrpcStatus::StatusCode::Aborted);
    ASSERT_EQ(asyncStatus, QGrpcStatus::StatusCode::Aborted);
    ASSERT_STREQ(result.testFieldString().toStdString().c_str(), "Result not changed by echo");
    testClient->deleteLater();
}

TEST_P(ClientTest, StringEchoDeferredAsyncAbortTest)
{
    auto testClient = (*GetParam())();
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("sleep");
    QEventLoop waiter;
    QGrpcCallReplyShared reply = testClient->testMethod(request);

    result.setTestFieldString("Result not changed by echo");
    bool errorCalled = false;
    reply = testClient->testMethod(request);
    QObject::connect(reply.get(), &QGrpcCallReply::finished, &m_app, [reply, &result, &waiter]() {
        result = reply->read<SimpleStringMessage>();
        waiter.quit();
    });
    QObject::connect(reply.get(), &QGrpcCallReply::error, [&errorCalled]() {
        errorCalled = true;
    });

    QTimer::singleShot(500, reply.get(), &QGrpcCallReply::abort);
    QTimer::singleShot(5000, &waiter, &QEventLoop::quit);

    waiter.exec();

    ASSERT_STREQ(result.testFieldString().toStdString().c_str(), "Result not changed by echo");
    ASSERT_TRUE(errorCalled);
    testClient->deleteLater();
}

TEST_P(ClientTest, StringEchoStreamTest)
{
    auto testClient = (*GetParam())();
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("Stream");

    QEventLoop waiter;

    int i = 0;
    auto stream = testClient->streamTestMethodServerStream(request);
    QObject::connect(stream.get(), &QGrpcStream::messageReceived, &m_app, [&result, &i, &waiter, stream]() {
        SimpleStringMessage ret = stream->read<SimpleStringMessage>();

        ++i;

        result.setTestFieldString(result.testFieldString() + ret.testFieldString());

        if (i == 4) {
            waiter.quit();
        }
    });


    QTimer::singleShot(20000, &waiter, &QEventLoop::quit);
    waiter.exec();

    ASSERT_EQ(i, 4);
    ASSERT_STREQ(result.testFieldString().toStdString().c_str(), "Stream1Stream2Stream3Stream4");
    testClient->deleteLater();
}

TEST_P(ClientTest, StringEchoStreamAbortTest)
{
    auto testClient = (*GetParam())();
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("Stream");

    QEventLoop waiter;

    int i = 0;
    auto stream = testClient->streamTestMethodServerStream(request);
    QObject::connect(stream.get(), &QGrpcStream::messageReceived, &m_app, [&result, &i, &waiter, stream]() {
        SimpleStringMessage ret = stream->read<SimpleStringMessage>();
        ++i;

        result.setTestFieldString(result.testFieldString() + ret.testFieldString());

        if (i == 3) {
            stream->abort();
            QTimer::singleShot(4000, &waiter, &QEventLoop::quit);
        }
    });

    QTimer::singleShot(20000, &waiter, &QEventLoop::quit);
    waiter.exec();

    ASSERT_EQ(i, 3);
    ASSERT_STREQ(result.testFieldString().toStdString().c_str(), "Stream1Stream2Stream3");
    testClient->deleteLater();
}

TEST_P(ClientTest, StringEchoStreamAbortByTimerTest)
{
    auto testClient = (*GetParam())();
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("Stream");

    QEventLoop waiter;


    int i = 0;
    auto stream = testClient->streamTestMethodServerStream(request);
    QTimer::singleShot(3500, stream.get(), [stream]() {
        stream->abort();
    });

    bool isFinished = false;
    QObject::connect(stream.get(), &QtProtobuf::QGrpcStream::finished, [&isFinished]() {
        isFinished = true;
    });

    bool isError = false;
    QObject::connect(stream.get(), &QtProtobuf::QGrpcStream::error, [&isError]() {
        isError = true;
    });

    QObject::connect(stream.get(), &QGrpcStream::messageReceived, &m_app, [&result, &i, stream]() {
        SimpleStringMessage ret = stream->read<SimpleStringMessage>();
        ++i;

        result.setTestFieldString(result.testFieldString() + ret.testFieldString());
    });

    QTimer::singleShot(5000, &waiter, &QEventLoop::quit);
    waiter.exec();

    ASSERT_EQ(i, 3);
    ASSERT_STREQ(result.testFieldString().toStdString().c_str(), "Stream1Stream2Stream3");
    ASSERT_TRUE(isFinished);
    ASSERT_TRUE(!isError);
    testClient->deleteLater();
}

TEST_P(ClientTest, StringEchoStreamTestRet)
{
    auto testClient = (*GetParam())();
    SimpleStringMessage request;
    QPointer<SimpleStringMessage> result(new SimpleStringMessage);

    request.setTestFieldString("Stream");

    QEventLoop waiter;

    testClient->streamTestMethodServerStream(request, result);

    int i = 0;
    QObject::connect(result.data(), &SimpleStringMessage::testFieldStringChanged, &m_app, [&i]() {
        i++;
    });

    QTimer::singleShot(20000, &waiter, &QEventLoop::quit);
    waiter.exec();

    ASSERT_EQ(i, 4);
    ASSERT_STREQ(result->testFieldString().toStdString().c_str(), "Stream4");
    delete result;
    testClient->deleteLater();
}


TEST_P(ClientTest, HugeBlobEchoStreamTest)
{
    auto testClient = (*GetParam())();
    BlobMessage result;
    BlobMessage request;
    QFile testFile("testfile");
    ASSERT_TRUE(testFile.open(QFile::ReadOnly));

    request.setTestBytes(testFile.readAll());
    QByteArray dataHash = QCryptographicHash::hash(request.testBytes(), QCryptographicHash::Sha256);
    QEventLoop waiter;

    auto stream = testClient->streamTestMethodBlobServerStream(request);

    QObject::connect(stream.get(), &QGrpcStream::messageReceived, &m_app, [&result, &waiter, stream]() {
        BlobMessage ret = stream->read<BlobMessage>();
        result.setTestBytes(ret.testBytes());
        waiter.quit();
    });


    QTimer::singleShot(20000, &waiter, &QEventLoop::quit);
    waiter.exec();

    QByteArray returnDataHash = QCryptographicHash::hash(result.testBytes(), QCryptographicHash::Sha256);
    ASSERT_TRUE(returnDataHash == dataHash);
    testClient->deleteLater();
}

TEST_P(ClientTest, StatusMessageAsyncTest)
{
    auto testClient = (*GetParam())();
    SimpleStringMessage request(QString{"Some status message"});
    QGrpcStatus::StatusCode asyncStatus = QGrpcStatus::StatusCode::Ok;
    QEventLoop waiter;
    QString statusMessage;

    QGrpcCallReplyShared reply = testClient->testMethodStatusMessage(request);
    QObject::connect(reply.get(), &QGrpcCallReply::error, [&asyncStatus, &waiter, &statusMessage](const QGrpcStatus &status) {
        asyncStatus = status.code();
        statusMessage = status.message();
        waiter.quit();
    });

    QTimer::singleShot(20000, &waiter, &QEventLoop::quit);
    waiter.exec();

    ASSERT_STREQ(statusMessage.toStdString().c_str(), request.testFieldString().toStdString().c_str());
    testClient->deleteLater();
}

TEST_P(ClientTest, StatusMessageClientAsyncTest)
{
    auto testClient = (*GetParam())();
    SimpleStringMessage request(QString{"Some status message"});
    QGrpcStatus::StatusCode asyncStatus = QGrpcStatus::StatusCode::Ok;
    QEventLoop waiter;
    QString statusMessage;

    QObject::connect(testClient, &TestServiceClient::error, [&asyncStatus, &waiter, &statusMessage](const QGrpcStatus &status) {
        asyncStatus = status.code();
        statusMessage = status.message();
        waiter.quit();
    });

    testClient->testMethodStatusMessage(request);

    QTimer::singleShot(20000, &waiter, &QEventLoop::quit);
    waiter.exec();

    ASSERT_STREQ(statusMessage.toStdString().c_str(), request.testFieldString().toStdString().c_str());
    testClient->deleteLater();
}

TEST_P(ClientTest, StatusMessageClientSyncTest)
{
    auto testClient = (*GetParam())();
    SimpleStringMessage request(QString{"Some status message"});
    QPointer<SimpleStringMessage> ret(new SimpleStringMessage);
    QGrpcStatus::StatusCode asyncStatus = QGrpcStatus::StatusCode::Ok;
    QEventLoop waiter;
    QString statusMessage;

    QObject::connect(testClient, &TestServiceClient::error, [&asyncStatus, &waiter, &statusMessage](const QGrpcStatus &status) {
        asyncStatus = status.code();
        statusMessage = status.message();
        waiter.quit();
    });

    testClient->testMethodStatusMessage(request, ret);
    QTimer::singleShot(20000, &waiter, &QEventLoop::quit);
    waiter.exec();

    ASSERT_STREQ(statusMessage.toStdString().c_str(), request.testFieldString().toStdString().c_str());
    delete ret;
    testClient->deleteLater();
}

TEST_P(ClientTest, StatusMessageClientSyncTestReturnedStatus)
{
    auto testClient = (*GetParam())();
    SimpleStringMessage request(QString{"Some status message"});
    QPointer<SimpleStringMessage> ret(new SimpleStringMessage);
    QEventLoop waiter;
    QString statusMessage;

    QGrpcStatus status = testClient->testMethodStatusMessage(request, ret);

    ASSERT_STREQ(status.message().toStdString().c_str(), request.testFieldString().toStdString().c_str());
    delete ret;
    testClient->deleteLater();
}


TEST_F(ClientTest, ClientSyncTestUnattachedChannel)
{
    TestServiceClient testClient;
    SimpleStringMessage request(QString{"Some status message"});
    QPointer<SimpleStringMessage> ret(new SimpleStringMessage);
    QEventLoop waiter;

    QGrpcStatus status = testClient.testMethodStatusMessage(request, ret);

    ASSERT_EQ(status.code(), QGrpcStatus::Unknown);
    ASSERT_STREQ("Serializing failed. Serializer is not ready", status.message().toStdString().c_str());
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
    ASSERT_STREQ("Serializing failed. Serializer is not ready", asyncStatus.message().toStdString().c_str());
    delete ret;
}

TEST_P(ClientTest, AsyncReplyStreamTest)
{
    auto testClient = (*GetParam())();
    QTimer callTimeout;
    SimpleStringMessage request(QString{"Some status message"});
    QGrpcStatus::StatusCode asyncStatus = QGrpcStatus::StatusCode::Ok;
    QEventLoop waiter;
    QString statusMessage;

    QObject::connect(&callTimeout, &QTimer::timeout, &waiter, &QEventLoop::quit);
    callTimeout.setInterval(5000);
    auto reply = testClient->testMethodStatusMessage(request);

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

    reply = testClient->testMethod(request);
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

    reply = testClient->testMethod(request);
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
    testClient->deleteLater();
}

TEST_P(ClientTest, MultipleStreamsTest)
{
    auto testClient = (*GetParam())();
    SimpleStringMessage result;
    SimpleStringMessage request;
    QEventLoop waiter;
    request.setTestFieldString("Stream");

    auto stream = testClient->streamTestMethodServerStream(request);
    auto streamNext = testClient->streamTestMethodServerStream(request);

    ASSERT_EQ(stream, streamNext);

    int i = 0;
    QObject::connect(stream.get(), &QGrpcStream::messageReceived, &m_app, [&result, &i, stream]() {
        SimpleStringMessage ret = stream->read<SimpleStringMessage>();
        ++i;

        result.setTestFieldString(result.testFieldString() + ret.testFieldString());
    });

    QTimer::singleShot(10000, &waiter, &QEventLoop::quit);
    waiter.exec();

    ASSERT_EQ(i, 4);
    ASSERT_STREQ(result.testFieldString().toStdString().c_str(), "Stream1Stream2Stream3Stream4");
    testClient->deleteLater();
}

TEST_P(ClientTest, MultipleStreamsCancelTest)
{
    auto testClient = (*GetParam())();
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("Stream");

    auto stream = testClient->streamTestMethodServerStream(request);
    auto streamNext = testClient->streamTestMethodServerStream(request);

    ASSERT_EQ(stream, streamNext);

    bool isFinished = false;
    QObject::connect(stream.get(), &QtProtobuf::QGrpcStream::finished, [&isFinished]() {
        isFinished = true;
    });

    bool isFinishedNext = false;
    QObject::connect(streamNext.get(), &QtProtobuf::QGrpcStream::finished, [&isFinishedNext]() {
        isFinishedNext = true;
    });

    streamNext->abort();

    ASSERT_TRUE(isFinished);
    ASSERT_TRUE(isFinishedNext);

    stream = testClient->streamTestMethodServerStream(request);
    ASSERT_NE(stream, streamNext);

    streamNext = testClient->streamTestMethodServerStream(request);

    ASSERT_EQ(stream, streamNext);

    isFinished = false;
    QObject::connect(stream.get(), &QtProtobuf::QGrpcStream::finished, [&isFinished]() {
        isFinished = true;
    });

    isFinishedNext = false;
    QObject::connect(streamNext.get(), &QtProtobuf::QGrpcStream::finished, [&isFinishedNext]() {
        isFinishedNext = true;
    });

    stream->abort();

    ASSERT_TRUE(isFinished);
    ASSERT_TRUE(isFinishedNext);
    testClient->deleteLater();
}

TEST_P(ClientTest, NonCompatibleArgRetTest)
{
    auto testClient = (*GetParam())();
    SimpleIntMessage request(2048);
    QPointer<SimpleStringMessage> result(new SimpleStringMessage);
    ASSERT_TRUE(testClient->testMethodNonCompatibleArgRet(request, result) == QGrpcStatus::Ok);
    ASSERT_STREQ(result->testFieldString().toStdString().c_str(), "2048");
    delete result;
    testClient->deleteLater();
}

TEST_P(ClientTest, StringEchoThreadTest)
{
    auto testClient = (*GetParam())();
    SimpleStringMessage request;
    QPointer<SimpleStringMessage> result(new SimpleStringMessage);
    request.setTestFieldString("Hello beach from thread!");
    bool ok = false;
    std::shared_ptr<QThread> thread(QThread::create([&](){
        ok = testClient->testMethod(request, result) == QGrpcStatus::Ok;
    }));

    thread->start();
    QEventLoop wait;
    QTimer::singleShot(2000, &wait, &QEventLoop::quit);
    wait.exec();

    ASSERT_TRUE(ok);
    ASSERT_STREQ(result->testFieldString().toStdString().c_str(), "Hello beach from thread!");
    delete result;

    //Delete result pointer in between call operations
    result = new SimpleStringMessage();
    ok = false;
    thread.reset(QThread::create([&](){
        ok = testClient->testMethod(request, result) == QGrpcStatus::Ok;
    }));

    thread->start();
    delete result;
    QTimer::singleShot(2000, &wait, &QEventLoop::quit);
    wait.exec();

    ASSERT_TRUE(!ok);
    testClient->deleteLater();
}


TEST_P(ClientTest, StringEchoAsyncThreadTest)
{
    auto testClient = (*GetParam())();
    SimpleStringMessage request;
    SimpleStringMessage result;
    request.setTestFieldString("Hello beach from thread!");

    bool threadsOk = true;
    bool replyDestroyed = true;
    std::shared_ptr<QThread> thread(QThread::create([&](){
        QEventLoop waiter;
        QThread *validThread = QThread::currentThread();
        QGrpcCallReplyShared reply = testClient->testMethod(request);
        QObject::connect(reply.get(), &QObject::destroyed, [&replyDestroyed]{replyDestroyed = true;});
        QObject::connect(reply.get(), &QGrpcCallReply::finished, &waiter, [reply, &result, &waiter, &threadsOk, validThread]() {
            threadsOk &= reply->thread() != QThread::currentThread();
            threadsOk &= validThread == QThread::currentThread();
            result = reply->read<SimpleStringMessage>();
            waiter.quit();
        });
        threadsOk &= reply->thread() != QThread::currentThread();
        waiter.exec();
    }));

    thread->start();
    QEventLoop wait;
    QTimer::singleShot(2000, &wait, &QEventLoop::quit);
    wait.exec();
    ASSERT_TRUE(replyDestroyed);
    ASSERT_TRUE(threadsOk);
    ASSERT_STREQ(result.testFieldString().toStdString().c_str(), "Hello beach from thread!");
    testClient->deleteLater();
}

TEST_P(ClientTest, StringEchoStreamThreadTest)
{
    auto testClient = (*GetParam())();
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("Stream");

    int i = 0;
    bool threadsOk = true;
    std::shared_ptr<QThread> thread(QThread::create([&](){
        QEventLoop waiter;
        QThread *validThread = QThread::currentThread();
        auto stream = testClient->streamTestMethodServerStream(request);
        QObject::connect(stream.get(), &QGrpcStream::messageReceived, &waiter, [&result, &i, &waiter, stream, &threadsOk, validThread]() {
            SimpleStringMessage ret = stream->read<SimpleStringMessage>();
            result.setTestFieldString(result.testFieldString() + ret.testFieldString());
            ++i;
            if (i == 4) {
                waiter.quit();
            }
            threadsOk &= stream->thread() != QThread::currentThread();
            threadsOk &= validThread == QThread::currentThread();
        });

        threadsOk &= stream->thread() != QThread::currentThread();
        QTimer::singleShot(20000, &waiter, &QEventLoop::quit);
        waiter.exec();
    }));

    thread->start();
    QEventLoop wait;
    QObject::connect(thread.get(), &QThread::finished, &wait, [&wait]{ wait.quit(); });
    QTimer::singleShot(20000, &wait, &QEventLoop::quit);
    wait.exec();

    ASSERT_TRUE(threadsOk);
    ASSERT_EQ(i, 4);
    ASSERT_STREQ(result.testFieldString().toStdString().c_str(), "Stream1Stream2Stream3Stream4");
    testClient->deleteLater();
}

TEST_F(ClientTest, AttachChannelThreadTest)
{
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    std::shared_ptr<QGrpcHttp2Channel> channel;
    std::shared_ptr<QThread> thread(QThread::create([&](){
        channel = std::make_shared<QGrpcHttp2Channel>(m_echoServerAddress, QGrpcInsecureCallCredentials() | QGrpcInsecureChannelCredentials());
    }));
    thread->start();
    QThread::msleep(1000);
    TestServiceClient testClient;
    EXPECT_THROW(testClient.attachChannel(channel), std::runtime_error);
}

TEST_P(ClientTest, StreamCancelWhileErrorTimeoutTest)
{
    auto *testClient = (*GetParam())();;
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("Stream");

    QEventLoop waiter;

    bool ok = false;
    auto stream = testClient->streamTestMethodServerStream(request);
    QObject::connect(stream.get(), &QGrpcStream::finished, &m_app, [&ok, &waiter]() {
        ok = true;
        waiter.quit();
    });
    stream->abort();
    stream.reset();

    QTimer::singleShot(5000, &waiter, &QEventLoop::quit);
    waiter.exec();

    ASSERT_TRUE(ok);
}

INSTANTIATE_TEST_SUITE_P(ClientTest, ClientTest, ::testing::ValuesIn(ClientTest::clientCreators));
