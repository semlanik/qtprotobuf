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
#include "qtprotobuf.h"
#include "insecurecredentials.h"

#include <QTimer>

#include <QCoreApplication>
#include <gtest/gtest.h>

using namespace qtprotobufnamespace::tests;
using namespace qtprotobuf;

class ClientTest : public ::testing::Test
{
protected:
    static void SetUpTestCase() {
        QtProtobuf::init();
        SimpleStringMessage::registerTypes();   
    }
};

TEST_F(ClientTest, CheckMethodsGeneration)
{
    //Dummy compile time check of functions generation and interface compatibility
    TestServiceClient testClient;
    SimpleStringMessage result;
    SimpleStringMessage request;
    testClient.testMethod(result, request);
    testClient.testMethod(result);
    testClient.testMethod(result, &testClient, [](AsyncReply*){});
}

TEST_F(ClientTest, StringEchoTest)
{
    int argc = 0;
    QCoreApplication app(argc, nullptr);
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<Http2Channel>("localhost", 50051, InsecureCredentials()));
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("Hello beach!");
    ASSERT_TRUE(testClient.testMethod(request, result));
    ASSERT_STREQ(result.testFieldString().toStdString().c_str(), "Hello beach!");
}

TEST_F(ClientTest, StringEchoAsyncTest)
{
    int argc = 0;
    QCoreApplication app(argc, nullptr);
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<Http2Channel>("localhost", 50051, InsecureCredentials()));
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("Hello beach!");
    QEventLoop waiter;

    AsyncReply* reply = testClient.testMethod(request);
    QObject::connect(reply, &AsyncReply::finished, &app, [reply, &result, &waiter, &testClient]() {
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
    int argc = 0;
    QCoreApplication app(argc, nullptr);
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<Http2Channel>("localhost", 50051, InsecureCredentials()));
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("Hello beach!");
    QEventLoop waiter;
    testClient.testMethod(request, &app, [&result, &waiter, &testClient](AsyncReply *reply) {
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
    int argc = 0;
    QCoreApplication app(argc, nullptr);
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<Http2Channel>("localhost", 50051, InsecureCredentials()));
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("sleep");
    QEventLoop waiter;

    bool errorCalled = false;
    AsyncReply* reply = testClient.testMethod(request);
    result.setTestFieldString("Result not changed by echo");
    QObject::connect(reply, &AsyncReply::finished, &app, [reply, &result, &waiter, &testClient]() {
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
    QObject::connect(reply, &AsyncReply::finished, &app, [reply, &result, &waiter, &testClient]() {
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
    int argc = 0;
    QCoreApplication app(argc, nullptr);
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<Http2Channel>("localhost", 50051, InsecureCredentials()));
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("Stream");

    QEventLoop waiter;

    int i = 0;
    QObject::connect(&testClient, &TestServiceClient::testMethodServerStreamUpdated, &app, [&result, &i, &waiter](const SimpleStringMessage& ret) {
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
    int argc = 0;
    QCoreApplication app(argc, nullptr);
    TestServiceClient testClient;
    testClient.attachChannel(std::make_shared<Http2Channel>("localhost", 50051, InsecureCredentials()));
    SimpleStringMessage result;
    SimpleStringMessage request;
    request.setTestFieldString("Stream");

    QEventLoop waiter;

    testClient.subscribeTestMethodServerStreamUpdates(request, result);

    int i = 0;
    QObject::connect(&result, &SimpleStringMessage::testFieldStringChanged, &app, [&i]() {
        i++;
    });

    QTimer::singleShot(20000, &waiter, &QEventLoop::quit);
    waiter.exec();

    ASSERT_EQ(i, 4);
    ASSERT_STREQ(result.testFieldString().toStdString().c_str(), "Stream4");
    ASSERT_EQ(testClient.lastError(), AbstractChannel::StatusCodes::Ok);
}
