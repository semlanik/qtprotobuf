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
#include <QGrpcInsecureCallCredentials>
#include <QGrpcInsecureChannelCredentials>

#include <QTimer>
#include <QFile>
#include <QCryptographicHash>
#include <QThread>
#include <QCoreApplication>
#include <QProcess>
#include <QProtobufSerializer>
#include <QDebug>
#include <QSignalSpy>

#include <QtTest/QtTest>

using namespace qtprotobufnamespace::tests;

typedef TestServiceClient* createTestServiceClientFunc();

const QUrl EchoServerAddress("http://localhost:50051", QUrl::StrictMode);
class QtGrpcClientTest : public QObject
{
    Q_OBJECT
public:
    QtGrpcClientTest(QObject *parent = nullptr) : QObject(parent)
    {
        qRegisterProtobufTypes();
        m_serverProcess.setProgram(QT_GRPC_SERVER_EXECUTABLE);
        m_serverProcess.start();
        m_serverProcess.waitForStarted();

        connect(&m_serverProcess, &QProcess::readyReadStandardError, &m_serverStarupWaiter, [this]{
            if (m_serverStarupWaiter.isRunning())
                m_serverStarupWaiter.quit();
        });
        connect(&m_serverProcess, &QProcess::readyReadStandardOutput, &m_serverStarupWaiter, [this]{
            if (m_serverStarupWaiter.isRunning())
                m_serverStarupWaiter.quit();
        });

        connect(&m_serverProcess, &QProcess::readyReadStandardError, this, [this]{
            qDebug() << ":" << "SERVER_ERROR" << m_serverProcess.readAllStandardError();
        });
        connect(&m_serverProcess, &QProcess::readyReadStandardOutput, this, [this]{
            qDebug() << ":" << "SERVER_MESSAGE" << m_serverProcess.readAllStandardOutput();
        });

        m_serverStarupWaiter.exec();
        qDebug() << "Server started";
    }

    virtual ~QtGrpcClientTest()
    {
        m_serverProcess.terminate();
        if(!m_serverProcess.waitForFinished(5000)) {
            m_serverProcess.kill();
        }
    }

    static TestServiceClient *createHttp2Client() {
        auto *c = new TestServiceClient();
        c->attachChannel(std::make_shared<QGrpcHttp2Channel>(EchoServerAddress, QGrpcInsecureChannelCredentials() | QGrpcInsecureCallCredentials()));
        return c;
    }

#ifdef QT_PROTOBUF_NATIVE_GRPC_CHANNEL
    static TestServiceClient * createGrpcSocketClient() {
        auto *c = new TestServiceClient();
        c->attachChannel(std::make_shared<QGrpcChannel>(QtGrpcClientTest::m_echoServerSocket, grpc::InsecureChannelCredentials()));
        return c;
    }

    static TestServiceClient * createGrpcHttpClient() {
        auto *c = new TestServiceClient();
        c->attachChannel(std::make_shared<QGrpcChannel>(QtGrpcClientTest::EchoServerAddressNative, grpc::InsecureChannelCredentials()));
        return c;
    }
    static const QString m_echoServerSocket;
    static const QString EchoServerAddressNative;
#endif

    static createTestServiceClientFunc *clientCreators[];
private slots:
    void CheckMethodsGeneration();
    void StringEchoTest();
    void StringEchoAsyncTest();
    void StringEchoAsync2Test();
    void StringEchoImmediateAsyncAbortTest();
    void StringEchoDeferredAsyncAbortTest();
    void StringEchoStreamTest();
    void StringEchoStreamAbortTest();
    void StringEchoStreamAbortByTimerTest();
    void StringEchoStreamTestRet();
    void HugeBlobEchoStreamTest();
    void StatusMessageAsyncTest();
    void StatusMessageClientAsyncTest();
    void StatusMessageClientSyncTest();
    void StatusMessageClientSyncTestReturnedStatus();
    void ClientSyncTestUnattachedChannel();
    void ClientSyncTestUnattachedChannelSignal();
    void AsyncReplyStreamTest();
    void MultipleStreamsTest();
    void MultipleStreamsCancelTest();
    void NonCompatibleArgRetTest();
    void StringEchoThreadTest();
    void StringEchoAsyncThreadTest();
    void StringEchoStreamThreadTest();
    void AttachChannelThreadTest();
    void StreamCancelWhileErrorTimeoutTest();
protected:
    QProcess m_serverProcess;
    QEventLoop m_serverStarupWaiter;
};


#ifdef QT_PROTOBUF_NATIVE_GRPC_CHANNEL
const QString QtGrpcClientTest::EchoServerAddressNative("localhost:50051");
const QString QtGrpcClientTest::m_echoServerSocket("unix:///tmp/test.sock");
#endif

createTestServiceClientFunc* QtGrpcClientTest::clientCreators[]{
    QtGrpcClientTest::createHttp2Client,
#ifdef QT_PROTOBUF_NATIVE_GRPC_CHANNEL
    QtGrpcClientTest::createGrpcHttpClient,
    QtGrpcClientTest::createGrpcSocketClient,
#endif
};

void QtGrpcClientTest::CheckMethodsGeneration()
{
    //Dummy compile time check of functions generation and interface compatibility
//    TestServiceClient testClient;
//    testClient.attachChannel(std::make_shared<QGrpcHttp2Channel>(QUrl(), QGrpcInsecureChannelCredentials() | QGrpcInsecureCallCredentials()));
//    SimpleStringMessage request;
//    QPointer<SimpleStringMessage> result(new SimpleStringMessage);
//    testClient.testMethod(request, result);
//    testClient.testMethod(request);
//    testClient.testMethod(request, &testClient, [](QGrpcCallReplyShared) {});
//    delete result;
}

void QtGrpcClientTest::StringEchoTest()
{
    auto testClient = createHttp2Client();
    SimpleStringMessage request;
    QPointer<SimpleStringMessage> result(new SimpleStringMessage);
    request.setTestFieldString(QStringLiteral("Hello Qt!"));
    QCOMPARE(testClient->testMethod(request, result), QGrpcStatus::Ok);
    QCOMPARE(result->testFieldString(), QStringLiteral("Hello Qt!"));
    delete result;
    testClient->deleteLater();
}

void QtGrpcClientTest::StringEchoAsyncTest()
{
    auto testClient = createHttp2Client();
    SimpleStringMessage request;
    SimpleStringMessage result;
    request.setTestFieldString(QStringLiteral("Hello Qt!"));

    QGrpcCallReplyShared reply = testClient->testMethod(request);
    QSignalSpy waiter(reply.get(), &QGrpcCallReply::finished);
    waiter.wait();
    result = reply->read<SimpleStringMessage>();
    reply->deleteLater();
    QCOMPARE(result.testFieldString(), QStringLiteral("Hello Qt!"));
    testClient->deleteLater();
}

void QtGrpcClientTest::StringEchoAsync2Test()
{
    auto testClient = createHttp2Client();
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString(QStringLiteral("Hello Qt!"));
    QEventLoop waiter;
    testClient->testMethod(request, this, [&result, &waiter](QGrpcCallReplyShared reply) {
        result = reply->read<SimpleStringMessage>();
        waiter.quit();
    });

    waiter.exec();
    QCOMPARE(result.testFieldString(), QStringLiteral("Hello Qt!"));
    testClient->deleteLater();
}

void QtGrpcClientTest::StringEchoImmediateAsyncAbortTest()
{
    auto testClient = createHttp2Client();
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("sleep");
    QEventLoop waiter;
    QGrpcCallReplyShared reply = testClient->testMethod(request);

    result.setTestFieldString("Result not changed by echo");
    QObject::connect(reply.get(), &QGrpcCallReply::finished, this, [&waiter, &result, reply]() {
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
        qDebug() << status.code() << ":" << status.message();
    });

    QTimer::singleShot(5000, &waiter, &QEventLoop::quit);
    reply->abort();
    waiter.exec();

    QCOMPARE(clientStatus, QGrpcStatus::StatusCode::Aborted);
    QCOMPARE(asyncStatus, QGrpcStatus::StatusCode::Aborted);
    QCOMPARE(result.testFieldString(), QStringLiteral("Result not changed by echo"));
    testClient->deleteLater();
}

void QtGrpcClientTest::StringEchoDeferredAsyncAbortTest()
{
    auto testClient = createHttp2Client();
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("sleep");
    QEventLoop waiter;
    QGrpcCallReplyShared reply = testClient->testMethod(request);

    result.setTestFieldString("Result not changed by echo");
    bool errorCalled = false;
    reply = testClient->testMethod(request);
    QObject::connect(reply.get(), &QGrpcCallReply::finished, this, [reply, &result, &waiter]() {
        result = reply->read<SimpleStringMessage>();
        waiter.quit();
    });
    QObject::connect(reply.get(), &QGrpcCallReply::error, [&errorCalled]() {
        errorCalled = true;
    });

    QTimer::singleShot(500, reply.get(), &QGrpcCallReply::abort);
    QTimer::singleShot(5000, &waiter, &QEventLoop::quit);

    waiter.exec();

    QCOMPARE(result.testFieldString(), QStringLiteral("Result not changed by echo"));
    QVERIFY(errorCalled);
    testClient->deleteLater();
}

void QtGrpcClientTest::StringEchoStreamTest()
{
    auto testClient = createHttp2Client();
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("Stream");

    QEventLoop waiter;

    int i = 0;
    auto stream = testClient->streamTestMethodServerStream(request);
    QObject::connect(stream.get(), &QGrpcStream::messageReceived, this, [&result, &i, &waiter, stream]() {
        SimpleStringMessage ret = stream->read<SimpleStringMessage>();

        ++i;

        result.setTestFieldString(result.testFieldString() + ret.testFieldString());

        if (i == 4) {
            waiter.quit();
        }
    });


    QTimer::singleShot(20000, &waiter, &QEventLoop::quit);
    waiter.exec();

    QCOMPARE(i, 4);
    QCOMPARE(result.testFieldString(), QStringLiteral("Stream1Stream2Stream3Stream4"));
    testClient->deleteLater();
}

void QtGrpcClientTest::StringEchoStreamAbortTest()
{
    auto testClient = createHttp2Client();
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("Stream");

    QEventLoop waiter;

    int i = 0;
    auto stream = testClient->streamTestMethodServerStream(request);
    QObject::connect(stream.get(), &QGrpcStream::messageReceived, this, [&result, &i, &waiter, stream]() {
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

    QCOMPARE(i, 3);
    QCOMPARE(result.testFieldString(), QStringLiteral("Stream1Stream2Stream3"));
    testClient->deleteLater();
}

void QtGrpcClientTest::StringEchoStreamAbortByTimerTest()
{
    auto testClient = createHttp2Client();
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
    QObject::connect(stream.get(), &QGrpcStream::finished, [&isFinished]() {
        isFinished = true;
    });

    bool isError = false;
    QObject::connect(stream.get(), &QGrpcStream::error, [&isError]() {
        isError = true;
    });

    QObject::connect(stream.get(), &QGrpcStream::messageReceived, this, [&result, &i, stream]() {
        SimpleStringMessage ret = stream->read<SimpleStringMessage>();
        ++i;

        result.setTestFieldString(result.testFieldString() + ret.testFieldString());
    });

    QTimer::singleShot(5000, &waiter, &QEventLoop::quit);
    waiter.exec();

    QCOMPARE(i, 3);
    QCOMPARE(result.testFieldString(), QStringLiteral("Stream1Stream2Stream3"));
    QVERIFY(isFinished);
    QVERIFY(!isError);
    testClient->deleteLater();
}

void QtGrpcClientTest::StringEchoStreamTestRet()
{
    auto testClient = createHttp2Client();
    SimpleStringMessage request;
    QPointer<SimpleStringMessage> result(new SimpleStringMessage);

    request.setTestFieldString("Stream");

    QEventLoop waiter;

    testClient->streamTestMethodServerStream(request, result);

    int i = 0;
    QObject::connect(result.data(), &SimpleStringMessage::testFieldStringChanged, this, [&i]() {
        i++;
    });

    QTimer::singleShot(20000, &waiter, &QEventLoop::quit);
    waiter.exec();

    QCOMPARE(i, 4);
    QCOMPARE(result->testFieldString(), QStringLiteral("Stream4"));
    delete result;
    testClient->deleteLater();
}

void QtGrpcClientTest::HugeBlobEchoStreamTest()
{
    auto testClient = createHttp2Client();
    BlobMessage result;
    BlobMessage request;
    QFile testFile("testfile");
    QVERIFY(testFile.open(QFile::ReadOnly));

    request.setTestBytes(testFile.readAll());
    QByteArray dataHash = QCryptographicHash::hash(request.testBytes(), QCryptographicHash::Sha256);
    QEventLoop waiter;

    auto stream = testClient->streamTestMethodBlobServerStream(request);

    QObject::connect(stream.get(), &QGrpcStream::messageReceived, this, [&result, &waiter, stream]() {
        BlobMessage ret = stream->read<BlobMessage>();
        result.setTestBytes(ret.testBytes());
        waiter.quit();
    });


    QTimer::singleShot(20000, &waiter, &QEventLoop::quit);
    waiter.exec();

    QByteArray returnDataHash = QCryptographicHash::hash(result.testBytes(), QCryptographicHash::Sha256);
    QVERIFY(returnDataHash == dataHash);
    testClient->deleteLater();
}

void QtGrpcClientTest::StatusMessageAsyncTest()
{
    auto testClient = createHttp2Client();
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

    QCOMPARE(statusMessage, request.testFieldString());
    testClient->deleteLater();
}

void QtGrpcClientTest::StatusMessageClientAsyncTest()
{
    auto testClient = createHttp2Client();
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

    QCOMPARE(statusMessage, request.testFieldString());
    testClient->deleteLater();
}

void QtGrpcClientTest::StatusMessageClientSyncTest()
{
    auto testClient = createHttp2Client();
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

    QCOMPARE(statusMessage, request.testFieldString());
    delete ret;
    testClient->deleteLater();
}

void QtGrpcClientTest::StatusMessageClientSyncTestReturnedStatus()
{
    auto testClient = createHttp2Client();
    SimpleStringMessage request(QString{"Some status message"});
    QPointer<SimpleStringMessage> ret(new SimpleStringMessage);
    QEventLoop waiter;
    QString statusMessage;

    QGrpcStatus status = testClient->testMethodStatusMessage(request, ret);

    QCOMPARE(status.message(), request.testFieldString());
    delete ret;
    testClient->deleteLater();
}

void QtGrpcClientTest::ClientSyncTestUnattachedChannel()
{
    TestServiceClient testClient;
    SimpleStringMessage request(QString{"Some status message"});
    QPointer<SimpleStringMessage> ret(new SimpleStringMessage);
    QEventLoop waiter;

    QGrpcStatus status = testClient.testMethodStatusMessage(request, ret);

    QCOMPARE(status.code(), QGrpcStatus::Unknown);
    QCOMPARE("Serializing failed. Serializer is not ready", status.message());
    delete ret;
}

void QtGrpcClientTest::ClientSyncTestUnattachedChannelSignal()
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

    QCOMPARE(asyncStatus, QGrpcStatus::Unknown);
    QCOMPARE("Serializing failed. Serializer is not ready", asyncStatus.message());
    delete ret;
}

void QtGrpcClientTest::AsyncReplyStreamTest()
{
    auto testClient = createHttp2Client();
    QTimer callTimeout;
    SimpleStringMessage request(QString{"Some status message"});
    QGrpcStatus::StatusCode asyncStatus = QGrpcStatus::StatusCode::Ok;
    QEventLoop waiter;
    QString statusMessage;

    QObject::connect(&callTimeout, &QTimer::timeout, &waiter, &QEventLoop::quit);
    callTimeout.setInterval(5000);
    auto reply = testClient->testMethodStatusMessage(request);

    reply->subscribe(this, []() {
        QVERIFY(false);
    },
    [&asyncStatus, &waiter, &statusMessage](const QGrpcStatus &status) {
        asyncStatus = status.code();
        statusMessage = status.message();
        waiter.quit();
    });

    callTimeout.start();
    waiter.exec();
    callTimeout.stop();

    QCOMPARE(statusMessage, request.testFieldString());

    SimpleStringMessage result;
    request.setTestFieldString(QStringLiteral("Hello Qt!"));

    reply = testClient->testMethod(request);
    reply->subscribe(this, [reply, &result, &waiter]() {
        result = reply->read<SimpleStringMessage>();
        waiter.quit();
    });

    callTimeout.start();
    waiter.exec();
    callTimeout.stop();
    QCOMPARE(result.testFieldString(), request.testFieldString());

    result.setTestFieldString("");
    request.setTestFieldString("Hello Qt1!");

    reply = testClient->testMethod(request);
    reply->subscribe(this, [reply, &result, &waiter]() {
        result = reply->read<SimpleStringMessage>();
        waiter.quit();
    }, []() {
        QVERIFY(false);
    });

    callTimeout.start();
    waiter.exec();
    callTimeout.stop();
    QCOMPARE(result.testFieldString(), request.testFieldString());
    testClient->deleteLater();
}

void QtGrpcClientTest::MultipleStreamsTest()
{
    auto testClient = createHttp2Client();
    SimpleStringMessage result;
    SimpleStringMessage request;
    QEventLoop waiter;
    request.setTestFieldString("Stream");

    auto stream = testClient->streamTestMethodServerStream(request);
    auto streamNext = testClient->streamTestMethodServerStream(request);

    QCOMPARE(stream, streamNext);

    int i = 0;
    QObject::connect(stream.get(), &QGrpcStream::messageReceived, this, [&result, &i, stream]() {
        SimpleStringMessage ret = stream->read<SimpleStringMessage>();
        ++i;

        result.setTestFieldString(result.testFieldString() + ret.testFieldString());
    });

    QTimer::singleShot(10000, &waiter, &QEventLoop::quit);
    waiter.exec();

    QCOMPARE(i, 4);
    QCOMPARE(result.testFieldString(), QStringLiteral("Stream1Stream2Stream3Stream4"));
    testClient->deleteLater();
}

void QtGrpcClientTest::MultipleStreamsCancelTest()
{
    auto testClient = createHttp2Client();
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("Stream");

    auto stream = testClient->streamTestMethodServerStream(request);
    auto streamNext = testClient->streamTestMethodServerStream(request);

    QCOMPARE(stream, streamNext);

    bool isFinished = false;
    QObject::connect(stream.get(), &QGrpcStream::finished, [&isFinished]() {
        isFinished = true;
    });

    bool isFinishedNext = false;
    QObject::connect(streamNext.get(), &QGrpcStream::finished, [&isFinishedNext]() {
        isFinishedNext = true;
    });

    streamNext->abort();

    QVERIFY(isFinished);
    QVERIFY(isFinishedNext);

    stream = testClient->streamTestMethodServerStream(request);
    if(stream == streamNext) {
        QFAIL("streamNext points to the same memory. New stream handler is not created.");
    }

    streamNext = testClient->streamTestMethodServerStream(request);

    QCOMPARE(stream, streamNext);

    isFinished = false;
    QObject::connect(stream.get(), &QGrpcStream::finished, [&isFinished]() {
        isFinished = true;
    });

    isFinishedNext = false;
    QObject::connect(streamNext.get(), &QGrpcStream::finished, [&isFinishedNext]() {
        isFinishedNext = true;
    });

    stream->abort();

    QVERIFY(isFinished);
    QVERIFY(isFinishedNext);
    testClient->deleteLater();
}

void QtGrpcClientTest::NonCompatibleArgRetTest()
{
    auto testClient = createHttp2Client();
    SimpleIntMessage request(2048);
    QPointer<SimpleStringMessage> result(new SimpleStringMessage);
    QVERIFY(testClient->testMethodNonCompatibleArgRet(request, result) == QGrpcStatus::Ok);
    QCOMPARE(result->testFieldString(), QStringLiteral("2048"));
    delete result;
    testClient->deleteLater();
}

void QtGrpcClientTest::StringEchoThreadTest()
{
    auto testClient = createHttp2Client();
    SimpleStringMessage request;
    QPointer<SimpleStringMessage> result(new SimpleStringMessage);
    request.setTestFieldString("Hello Qt from thread!");
    bool ok = false;
    std::shared_ptr<QThread> thread(QThread::create([&](){
        ok = testClient->testMethod(request, result) == QGrpcStatus::Ok;
    }));

    thread->start();
    QEventLoop wait;
    QTimer::singleShot(2000, &wait, &QEventLoop::quit);
    wait.exec();

    QVERIFY(ok);
    QCOMPARE(result->testFieldString(), QStringLiteral("Hello Qt from thread!"));
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

    QVERIFY(!ok);
    testClient->deleteLater();
}

void QtGrpcClientTest::StringEchoAsyncThreadTest()
{
    auto testClient = createHttp2Client();
    SimpleStringMessage request;
    SimpleStringMessage result;
    request.setTestFieldString("Hello Qt from thread!");

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
    QVERIFY(replyDestroyed);
    QVERIFY(threadsOk);
    QCOMPARE(result.testFieldString(), QStringLiteral("Hello Qt from thread!"));
    testClient->deleteLater();
}

void QtGrpcClientTest::StringEchoStreamThreadTest()
{
    auto testClient = createHttp2Client();
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

    QVERIFY(threadsOk);
    QCOMPARE(i, 4);
    QCOMPARE(result.testFieldString(), QStringLiteral("Stream1Stream2Stream3Stream4"));
    testClient->deleteLater();
}

void QtGrpcClientTest::AttachChannelThreadTest()
{
    std::shared_ptr<QGrpcHttp2Channel> channel;
    std::shared_ptr<QThread> thread(QThread::create([&](){
        channel = std::make_shared<QGrpcHttp2Channel>(EchoServerAddress, QGrpcInsecureCallCredentials() | QGrpcInsecureChannelCredentials());
    }));
    thread->start();
    QThread::msleep(1000);
    TestServiceClient testClient;
    QVERIFY_THROWS_EXCEPTION(std::runtime_error, testClient.attachChannel(channel));
}

void QtGrpcClientTest::StreamCancelWhileErrorTimeoutTest()
{
    auto *testClient = createHttp2Client();;
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("Stream");

    QEventLoop waiter;

    bool ok = false;
    auto stream = testClient->streamTestMethodServerStream(request);
    QObject::connect(stream.get(), &QGrpcStream::finished, this, [&ok, &waiter]() {
        ok = true;
        waiter.quit();
    });
    stream->abort();
    stream.reset();

    QTimer::singleShot(5000, &waiter, &QEventLoop::quit);
    waiter.exec();

    QVERIFY(ok);
}

QTEST_MAIN(QtGrpcClientTest)
#include "clienttest.moc"
