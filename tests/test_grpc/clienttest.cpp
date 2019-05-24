/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>
 *
 * This file is part of qtprotobuf project https://git.semlanik.org/semlanik/qtprotobuf
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

#include "testserviceclient.h"
#include "http2channel.h"
#include "insecurecredentials.h"
#include "blobmessage.h"

#include <QTimer>
#include <QFile>
#include <QCryptographicHash>

#include <QCoreApplication>
#include <gtest/gtest.h>

using namespace qtprotobufnamespace::tests;
using namespace qtprotobuf;

class ClientTest : public ::testing::Test
{
protected:
    static void SetUpTestCase() {
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
    SimpleStringMessage request;
    QPointer<SimpleStringMessage> result(new SimpleStringMessage);
    testClient.testMethod(request, result);
    testClient.testMethod(request);
    testClient.testMethod(request, &testClient, [](AsyncReply *){});
    delete result;
}

TEST_F(ClientTest, StringEchoTest)
{
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<Http2Channel>(m_echoServerAddress, InsecureCredentials()));
    SimpleStringMessage request;
    QPointer<SimpleStringMessage> result(new SimpleStringMessage);
    request.setTestFieldString("Hello beach!");
    ASSERT_TRUE(testClient.testMethod(request, result));
    ASSERT_STREQ(result->testFieldString().toStdString().c_str(), "Hello beach!");
    delete result;
}

TEST_F(ClientTest, StringEchoAsyncTest)
{
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<Http2Channel>(m_echoServerAddress, InsecureCredentials()));
    SimpleStringMessage request;
    SimpleStringMessage result;
    request.setTestFieldString("Hello beach!");
    QEventLoop waiter;

    AsyncReply *reply = testClient.testMethod(request);
    QObject::connect(reply, &AsyncReply::finished, &m_app, [reply, &result, &waiter, &testClient]() {
        if (testClient.lastError() == AbstractChannel::StatusCodes::Ok) {
            result = reply->read<SimpleStringMessage>();
        }
        waiter.quit();
    });

    waiter.exec();
    ASSERT_STREQ(result.testFieldString().toStdString().c_str(), "Hello beach!");
}

TEST_F(ClientTest, StringEchoAsync2Test)
{
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<Http2Channel>(m_echoServerAddress, InsecureCredentials()));
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("Hello beach!");
    QEventLoop waiter;
    testClient.testMethod(request, &m_app, [&result, &waiter, &testClient](AsyncReply *reply) {
        if (testClient.lastError() == AbstractChannel::StatusCodes::Ok) {
            result = reply->read<SimpleStringMessage>();
        }
        waiter.quit();
    });

    waiter.exec();
    ASSERT_STREQ(result.testFieldString().toStdString().c_str(), "Hello beach!");
}

TEST_F(ClientTest, StringEchoAsyncAbortTest)
{
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<Http2Channel>(m_echoServerAddress, InsecureCredentials()));
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("sleep");
    QEventLoop waiter;

    bool errorCalled = false;
    AsyncReply *reply = testClient.testMethod(request);
    result.setTestFieldString("Result not changed by echo");
    QObject::connect(reply, &AsyncReply::finished, &m_app, [reply, &result, &waiter, &testClient]() {
        if (testClient.lastError() == AbstractChannel::StatusCodes::Ok) {
            result = reply->read<SimpleStringMessage>();
        }
        waiter.quit();
    });

    QObject::connect(reply, &AsyncReply::error, reply, [&errorCalled](AbstractChannel::StatusCodes){
        errorCalled = true;
    });
    QTimer::singleShot(5000, &waiter, &QEventLoop::quit);
    reply->abort();

    waiter.exec();
    ASSERT_STREQ(result.testFieldString().toStdString().c_str(), "Result not changed by echo");
    ASSERT_EQ(testClient.lastError(), AbstractChannel::StatusCodes::Aborted);
    ASSERT_TRUE(errorCalled);


    errorCalled = false;
    reply = testClient.testMethod(request);
    QObject::connect(reply, &AsyncReply::finished, &m_app, [reply, &result, &waiter, &testClient]() {
        if (testClient.lastError() == AbstractChannel::StatusCodes::Ok) {
            result = reply->read<SimpleStringMessage>();
        }
        waiter.quit();
    });
    QObject::connect(reply, &AsyncReply::error, reply, [&errorCalled](AbstractChannel::StatusCodes){
        errorCalled = true;
    });

    QTimer::singleShot(2000, reply, &AsyncReply::abort);
    QTimer::singleShot(5000, &waiter, &QEventLoop::quit);

    waiter.exec();

    ASSERT_STREQ(result.testFieldString().toStdString().c_str(), "Result not changed by echo");
    ASSERT_EQ(testClient.lastError(), AbstractChannel::StatusCodes::Aborted);
    ASSERT_TRUE(errorCalled);
}

TEST_F(ClientTest, StringEchoStreamTest)
{
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<Http2Channel>(m_echoServerAddress, InsecureCredentials()));
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
    ASSERT_EQ(testClient.lastError(), AbstractChannel::StatusCodes::Ok);
}

TEST_F(ClientTest, StringEchoStreamTestRetUpdates)
{
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<Http2Channel>(m_echoServerAddress, InsecureCredentials()));
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
    ASSERT_EQ(testClient.lastError(), AbstractChannel::StatusCodes::Ok);
}


TEST_F(ClientTest, HugeBlobEchoStreamTest)
{
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<Http2Channel>(m_echoServerAddress, InsecureCredentials()));
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
    ASSERT_EQ(testClient.lastError(), AbstractChannel::StatusCodes::Ok);
}
