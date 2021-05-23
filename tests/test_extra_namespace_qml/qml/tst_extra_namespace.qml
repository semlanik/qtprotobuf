/*
 * MIT License
 *
 * Copyright (c) 2021 Alexey Edelev <semlanik@gmail.com>
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
import MyTestNamespace.qtprotobufnamespace.tests 1.0

TestCase {
    name: "Test extra namespace generation"
    SimpleStringMessage {
        id: stringMsg
        testFieldString: "Test string"
    }
    ComplexMessage {
        id: complexMsg
        testComplexField: SimpleStringMessage {
            id: innerMessage
            testFieldString: "inner"
        }
    }

    SimpleStringMessage {
        id: outerMessage
        testFieldString: "outer"
    }

    function test_simplesstringmessage() {
        compare(stringMsg.testFieldString, "Test string", "SimpleStringMessage")
    }

    function test_complexMessage() {
        compare(complexMsg.testComplexField, innerMessage, "Invalid object is inside complex message")
        compare(complexMsg.testComplexField.testFieldString, "inner", "Invalid value of object inside complex message")

        complexMsg.testComplexField = outerMessage
        compare(complexMsg.testComplexField, outerMessage, "Invalid object is inside complex message")
        compare(complexMsg.testComplexField.testFieldString, "outer", "Invalid value of object inside complex message")

        complexMsg.testComplexField = innerMessage
        compare(complexMsg.testComplexField, innerMessage, "Invalid object is inside complex message")
        compare(complexMsg.testComplexField.testFieldString, "inner", "Invalid value of object inside complex message")
    }
}
