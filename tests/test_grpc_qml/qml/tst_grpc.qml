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
    }

    SimpleIntMessage {
        id: intMsg
        testField: 1024
    }

    QtObject {
        id: returnMsg
        property string ret: serverStream.returnValue.testFieldString
    }

    GrpcStream {
        id: serverStream
        property bool ok: true
        property int updateCount: 0

        enabled: false
        client: TestServiceClient
        method: "testMethodServerStream"
        argument: stringMsg
        onMessageReceived: {
            ++updateCount;
            ok = ok && value.testFieldString === ("test_serverStream" + updateCount) && returnMsg.ret == ("test_serverStream" + updateCount)
        }
        onError: {
            console.log("Stream error: " + status.code + " " + status.message)
            ok = false;
        }
    }

    GrpcStream {
        id: serverStreamCancel
        property bool ok: true
        property int updateCount: 0

        enabled: false
        client: TestServiceClient
        method: "testMethodServerStream"
        argument: stringMsg
        onMessageReceived: {
            ++updateCount;
            ok = ok && value.testFieldString === "test_serverStreamCancel" + updateCount
            if (updateCount === 3) {
                serverStreamCancel.enabled = false;
            }
        }
        onError: {
            console.log("Stream error: " + status.code + " " + status.message)
            ok = false;
        }
    }

    GrpcStream {
        id: serverStreamInvalid
        property bool ok: false
        enabled: false
        client: TestServiceClient
        method: "testMethodServerStreamNotExist"
        argument: stringMsg
        onMessageReceived: {
            ok = false;
        }
        onError: {
            ok = status.code === GrpcStatus.Unimplemented;
        }
    }

    Loader {
        id: streamLoader
        active: false
        property bool ok: true
        property int updateCount: 0
        sourceComponent: Component {
            GrpcStream {
                enabled: true
                client: TestServiceClient
                method: "testMethodServerStream"
                argument: stringMsg
                onMessageReceived: {
                    ++streamLoader.updateCount;
                    streamLoader.ok = ok && value.testFieldString === ("test_1loader" + streamLoader.updateCount)
                }
                onError: {
                    console.log("Stream error: " + status.code + " " + status.message)
                    streamLoader.ok = false;
                }
            }
        }
    }

    function test_stringEchoTest() {
        stringMsg.testFieldString = "test_stringEchoTest";
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
        stringMsg.testFieldString = "test_statusTest";
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
    }

    function test_serverStream() {
        stringMsg.testFieldString = "test_serverStream";
        serverStream.enabled = true;
        wait(20000);
        compare(serverStream.ok, true, "Stream data failed")
        compare(serverStream.updateCount, 4, "Stream failed, update was not called right amount times")
    }

    function test_serverStreamCancel() {
        stringMsg.testFieldString = "test_serverStreamCancel";
        serverStreamCancel.enabled = true;
        wait(20000);
        compare(serverStreamCancel.ok, true, "Stream data failed")
        compare(serverStreamCancel.updateCount, 3, "Stream failed, update was not called right amount times")
    }

    function test_serverStreamInvalid() {
        serverStreamInvalid.enabled = true;
        wait(500);
        compare(serverStreamInvalid.ok, true, "Stream data failed")
        compare(serverStreamInvalid.enabled, false, "Stream data failed")
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
        stringMsg.testFieldString = "test_1loader";
        streamLoader.active = true;
        wait(1500);
        streamLoader.active = false;
        wait(2000);
        compare(streamLoader.ok, true, "Stream data failed")
        compare(streamLoader.updateCount, 1, "Stream failed, update was not called right amount times")
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
