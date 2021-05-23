/*
 * MIT License
 *
 * Copyright (c) 2020 Alexey Edelev <semlanik@gmail.com>
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

import QtQuick 2.12
import QtTest 1.0

import QtProtobuf 0.6
import QtGrpc 0.6
import qtprotobufnamespace.tests 1.0

TestCase {
    name: "gRPC client qml test"
    SimpleStringMessage {
        id: stringMsg
        testFieldString: "Test string"
    }

    SimpleIntMessage {
        id: intMsg
        testField: 1024
    }

    QtObject {
        id: returnMsg
        property string ret: serverStreamSubscription.returnValue.testFieldString
    }

    GrpcSubscription {
        id: serverStreamSubscription
        property bool ok: true
        property int updateCount: 0

        enabled: false
        client: TestServiceClient
        method: "testMethodServerStream"
        argument: stringMsg
        onUpdated: {
            ++updateCount;
            ok = ok && value.testFieldString === ("Test string" + updateCount) && returnMsg.ret == ("Test string" + updateCount)
        }
        onError: {
            console.log("Subscription error: " + status.code + " " + status.message)
            ok = false;
        }
    }

    GrpcSubscription {
        id: serverStreamCancelSubscription
        property bool ok: true
        property int updateCount: 0

        enabled: false
        client: TestServiceClient
        method: "testMethodServerStream"
        argument: stringMsg
        onUpdated: {
            ++updateCount;
            ok = ok && value.testFieldString === "Test string" + updateCount
            if (updateCount === 3) {
                serverStreamCancelSubscription.enabled = false;
            }
        }
        onError: {
            console.log("Subscription error: " + status.code + " " + status.message)
            ok = false;
        }
    }

    GrpcSubscription {
        id: serverStreamInvalidSubscription
        property bool ok: false
        enabled: false
        client: TestServiceClient
        method: "testMethodServerStreamNotExist"
        argument: stringMsg
        onUpdated: {
            ok = false;
        }
        onError: {
            ok = status.code === GrpcStatus.Unimplemented;
        }
    }

    Loader {
        id: subscriptionLoader
        active: false
        sourceComponent: Component {
            GrpcSubscription {
                property bool ok: true
                property int updateCount: 0
                enabled: true
                client: TestServiceClient
                method: "testMethodServerStream"
                argument: stringMsg
                onUpdated: {
                    ++updateCount;
                    ok = ok && value.testFieldString === ("Test string" + updateCount)
                }
                onError: {
                    console.log("Subscription error: " + status.code + " " + status.message)
                    ok = false;
                }
            }
        }
    }

    function test_stringEchoTest() {
        var called = false;
        var errorCalled = false;
        TestServiceClient.testMethod(stringMsg, function(result) {
            called = result.testFieldString === stringMsg.testFieldString;
        }, function(status) {
            errorCalled = true
        })
        wait(300)
        compare(called && !errorCalled, true, "testMethod was not called proper way")
    }

    function test_statusTest() {
        var called = false;
        var errorCalled = false;
        TestServiceClient.testMethodStatusMessage(stringMsg, function(result) {
            called = true;
        }, function(status) {
            errorCalled = status.code === GrpcStatus.Unimplemented && status.message === stringMsg.testFieldString;
        })
        wait(300)
        compare(!called && errorCalled, true, "testMethodStatusMessage was not called proper way")
    }

    function test_stringEchoAsyncTest() {
        var called = false;
        var errorCalled = false;
        stringMsg.testFieldString = "sleep";
        TestServiceClient.testMethod(stringMsg, function(result) {
            called = result.testFieldString === stringMsg.testFieldString;
        }, function(status) {
            errorCalled = true
        })
        wait(300)
        compare(!called && !errorCalled, true, "testMethod was not called proper way")
        wait(1000)
        compare(called && !errorCalled, true, "testMethod was not called proper way")
        stringMsg.testFieldString = "Test string";
    }

    function test_serverStreamSubscription() {
        serverStreamSubscription.enabled = true;
        wait(20000);
        compare(serverStreamSubscription.ok, true, "Subscription data failed")
        compare(serverStreamSubscription.updateCount, 4, "Subscription failed, update was not called right amount times")
    }

    function test_serverStreamCancelSubscription() {
        serverStreamCancelSubscription.enabled = true;
        wait(20000);
        compare(serverStreamCancelSubscription.ok, true, "Subscription data failed")
        compare(serverStreamCancelSubscription.updateCount, 3, "Subscription failed, update was not called right amount times")
    }

    function test_serverStreamInvalidSubscription() {
        serverStreamInvalidSubscription.enabled = true;
        wait(500);
        compare(serverStreamInvalidSubscription.ok, true, "Subscription data failed")
        compare(serverStreamInvalidSubscription.enabled, false, "Subscription data failed")
    }

    function test_nonCompatibleArgRet() {
        var called = false;
        var errorCalled = false;
        TestServiceClient.testMethodNonCompatibleArgRet(intMsg, function(result) {
            called = result.testFieldString === "1024";
        }, function(status) {
            errorCalled = true
        })
        wait(300)
        compare(called && !errorCalled, true, "testMethodNonCompatibleArgRet was not called proper way")
    }

    function test_1loader() {//This test has to be called first to fail other tests in case if it fails
        subscriptionLoader.active = true;
        wait(1500);
        compare(subscriptionLoader.item.ok, true, "Subscription data failed")
        compare(subscriptionLoader.item.updateCount, 1, "Subscription failed, update was not called right amount times")
        subscriptionLoader.active = false;
    }

    SimpleStringMessage {
        id: returnStringMsg
    }

    function test_returnValueAsParameter() {
        var errorCalled = false;
        TestServiceClient.testMethod(stringMsg, returnStringMsg, function(status) {
            errorCalled = true
        })
        wait(300)
        compare(returnStringMsg.testFieldString == stringMsg.testFieldString && !errorCalled, true, "testMethod was not called proper way")
    }
}
