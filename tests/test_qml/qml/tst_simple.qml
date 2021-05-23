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

import QtQuick 2.12
import QtTest 1.0

import QtProtobuf 0.6
import qtprotobufnamespace.tests 1.0
import qtprotobufnamespace.tests.nested 1.0
import qtprotobufnamespace.tests.nested.NestedFieldMessage 1.0 as NestedFieldMessage_

TestCase {
    name: "Simple values assignment"
    SimpleBoolMessage {
        id: boolMsg
        testFieldBool: false
    }
    SimpleIntMessage {
        id: int32Msg
        testFieldInt: 2147483647
    }
    SimpleSIntMessage {
        id: sint32Msg
        testFieldInt: 2147483647
    }
    SimpleUIntMessage {
        id: uint32Msg
        testFieldInt: 4294967295
    }
    SimpleFixedInt32Message {
        id: fixed32Msg
        testFieldFixedInt32: 4294967295
    }
    SimpleSFixedInt32Message {
        id: sfixed32Msg
        testFieldFixedInt32: 2147483647
    }
    SimpleStringMessage {
        id: stringMsg
        testFieldString: "Test string"
    }

    MessageUpperCase {
        id: caseSenseMsg
    }

    MessageReserved {
        id: reservedMsg
    }

    PriorMessageUnderscoreField {
        id: underScoreMsg
    }

    LowerCaseMessageName {
        id: lowerCaseMsg
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

    NestedFieldMessage {
        id: nestedParent
        nested: nestedMsg
    }

    NestedFieldMessage_.NestedMessage {
        id: nestedMsg
        testFieldInt: 100
    }

    NoPackageMessage {
        id: noPackageMessageUser
        testField.testFieldInt: 42
    }

    function test_1initialization() {
        compare(int32Msg.testFieldInt, 2147483647, "SimpleIntMessage initialization")
        compare(sint32Msg.testFieldInt, 2147483647, "SimpleSIntMessage initialization")
        compare(uint32Msg.testFieldInt, 4294967295, "SimpleUIntMessage initialization")
        compare(fixed32Msg.testFieldFixedInt32, 4294967295, "SimpleFixedInt32Message initialization")
        compare(sfixed32Msg.testFieldFixedInt32, 2147483647, "SimpleSFixedInt32Message initialization")
    }

    function test_simpleboolmessage() {
        boolMsg.testFieldBool = true;
        compare(boolMsg.testFieldBool, true, "SimpleBoolMessage == true")
        boolMsg.testFieldBool = false;
        compare(boolMsg.testFieldBool, false, "SimpleBoolMessage == false")
    }

    function test_simpleintmessage() {
        int32Msg.testFieldInt = 0;
        compare(int32Msg.testFieldInt, 0, "SimpleIntMessage == 0")
        int32Msg.testFieldInt = -128;
        compare(int32Msg.testFieldInt, -128, "SimpleIntMessage == -128")
        int32Msg.testFieldInt = 127;
        compare(int32Msg.testFieldInt, 127, "SimpleIntMessage == 127")
        int32Msg.testFieldInt = -256;
        compare(int32Msg.testFieldInt, -256, "SimpleIntMessage == -256")
        int32Msg.testFieldInt = 255;
        compare(int32Msg.testFieldInt, 255, "SimpleIntMessage == 255")
        int32Msg.testFieldInt = -32768;
        compare(int32Msg.testFieldInt, -32768, "SimpleIntMessage == -32768")
        int32Msg.testFieldInt = 32767;
        compare(int32Msg.testFieldInt, 32767, "SimpleIntMessage == 32767")
        int32Msg.testFieldInt = -65536;
        compare(int32Msg.testFieldInt, -65536, "SimpleIntMessage == -65536")
        int32Msg.testFieldInt = 65535;
        compare(int32Msg.testFieldInt, 65535, "SimpleIntMessage == 65535")
        int32Msg.testFieldInt = -2147483648;
        compare(int32Msg.testFieldInt, -2147483648, "SimpleIntMessage == -2147483648")
        int32Msg.testFieldInt = 2147483647;
        compare(int32Msg.testFieldInt, 2147483647, "SimpleIntMessage == 2147483647")
    }

    function test_simplesintmessage() {
        sint32Msg.testFieldInt = 0;
        compare(sint32Msg.testFieldInt, 0, "SimpleSIntMessage == 0")
        sint32Msg.testFieldInt = -128;
        compare(sint32Msg.testFieldInt, -128, "SimpleSIntMessage == -128")
        sint32Msg.testFieldInt = 127;
        compare(sint32Msg.testFieldInt, 127, "SimpleSIntMessage == 127")
        sint32Msg.testFieldInt = -256;
        compare(sint32Msg.testFieldInt, -256, "SimpleSIntMessage == -256")
        sint32Msg.testFieldInt = 255;
        compare(sint32Msg.testFieldInt, 255, "SimpleSIntMessage == 255")
        sint32Msg.testFieldInt = -32768;
        compare(sint32Msg.testFieldInt, -32768, "SimpleSIntMessage == -32768")
        sint32Msg.testFieldInt = 32767;
        compare(sint32Msg.testFieldInt, 32767, "SimpleSIntMessage == 32767")
        sint32Msg.testFieldInt = -65536;
        compare(sint32Msg.testFieldInt, -65536, "SimpleSIntMessage == -65536")
        sint32Msg.testFieldInt = 65535;
        compare(sint32Msg.testFieldInt, 65535, "SimpleSIntMessage == 65535")
        sint32Msg.testFieldInt = -2147483648;
        compare(sint32Msg.testFieldInt, -2147483648, "SimpleSIntMessage == -2147483648")
        sint32Msg.testFieldInt = 2147483647;
        compare(sint32Msg.testFieldInt, 2147483647, "SimpleSIntMessage == 2147483647")
    }

    function test_simpleuintmessage() {
        uint32Msg.testFieldInt = 0;
        compare(uint32Msg.testFieldInt, 0, "SimpleUIntMessage == 0")
        uint32Msg.testFieldInt = 127;
        compare(uint32Msg.testFieldInt, 127, "SimpleUIntMessage == 127")
        uint32Msg.testFieldInt = 255;
        compare(uint32Msg.testFieldInt, 255, "SimpleUIntMessage == 255")
        uint32Msg.testFieldInt = 32767;
        compare(uint32Msg.testFieldInt, 32767, "SimpleUIntMessage == 32767")
        uint32Msg.testFieldInt = 65535;
        compare(uint32Msg.testFieldInt, 65535, "SimpleUIntMessage == 65535")
        uint32Msg.testFieldInt = 2147483647;
        compare(uint32Msg.testFieldInt, 2147483647, "SimpleUIntMessage == 2147483647")
        uint32Msg.testFieldInt = 4294967295;
        compare(uint32Msg.testFieldInt, 4294967295, "SimpleUIntMessage == 4294967295")
    }

    function test_simplefixed32message() {
        fixed32Msg.testFieldFixedInt32 = 0;
        compare(fixed32Msg.testFieldFixedInt32, 0, "SimpleFixedInt32Message == 0")
        fixed32Msg.testFieldFixedInt32 = 127;
        compare(fixed32Msg.testFieldFixedInt32, 127, "SimpleFixedInt32Message == 127")
        fixed32Msg.testFieldFixedInt32 = 255;
        compare(fixed32Msg.testFieldFixedInt32, 255, "SimpleFixedInt32Message == 255")
        fixed32Msg.testFieldFixedInt32 = 32767;
        compare(fixed32Msg.testFieldFixedInt32, 32767, "SimpleFixedInt32Message == 32767")
        fixed32Msg.testFieldFixedInt32 = 65535;
        compare(fixed32Msg.testFieldFixedInt32, 65535, "SimpleFixedInt32Message == 65535")
        fixed32Msg.testFieldFixedInt32 = 2147483647;
        compare(fixed32Msg.testFieldFixedInt32, 2147483647, "SimpleFixedInt32Message == 2147483647")
        fixed32Msg.testFieldFixedInt32 = 4294967295;
        compare(fixed32Msg.testFieldFixedInt32, 4294967295, "SimpleFixedInt32Message == 4294967295")
    }

    function test_simplesfixed32message() {
        sfixed32Msg.testFieldFixedInt32 = 0;
        compare(sfixed32Msg.testFieldFixedInt32, 0, "SimpleSFixedInt32Message == 0")
        sfixed32Msg.testFieldFixedInt32 = -128;
        compare(sfixed32Msg.testFieldFixedInt32, -128, "SimpleSFixedInt32Message == -128")
        sfixed32Msg.testFieldFixedInt32 = 127;
        compare(sfixed32Msg.testFieldFixedInt32, 127, "SimpleSFixedInt32Message == 127")
        sfixed32Msg.testFieldFixedInt32 = -256;
        compare(sfixed32Msg.testFieldFixedInt32, -256, "SimpleSFixedInt32Message == -256")
        sfixed32Msg.testFieldFixedInt32 = 255;
        compare(sfixed32Msg.testFieldFixedInt32, 255, "SimpleSFixedInt32Message == 255")
        sfixed32Msg.testFieldFixedInt32 = -32768;
        compare(sfixed32Msg.testFieldFixedInt32, -32768, "SimpleSFixedInt32Message == -32768")
        sfixed32Msg.testFieldFixedInt32 = 32767;
        compare(sfixed32Msg.testFieldFixedInt32, 32767, "SimpleSFixedInt32Message == 32767")
        sfixed32Msg.testFieldFixedInt32 = -65536;
        compare(sfixed32Msg.testFieldFixedInt32, -65536, "SimpleSFixedInt32Message == -65536")
        sfixed32Msg.testFieldFixedInt32 = 65535;
        compare(sfixed32Msg.testFieldFixedInt32, 65535, "SimpleSFixedInt32Message == 65535")
        sfixed32Msg.testFieldFixedInt32 = -2147483648;
        compare(sfixed32Msg.testFieldFixedInt32, -2147483648, "SimpleSFixedInt32Message == -2147483648")
        sfixed32Msg.testFieldFixedInt32 = 2147483647;
        compare(sfixed32Msg.testFieldFixedInt32, 2147483647, "SimpleSFixedInt32Message == 2147483647")
    }

    function test_simplesstringmessage() {
        compare(stringMsg.testFieldString, "Test string", "SimpleStringMessage")
    }

    function test_reservedNames() {
        reservedMsg.id_proto = 34;
        reservedMsg.import_proto = 35;
        reservedMsg.property_proto = 36;
        compare(reservedMsg.id_proto, 34, "reservedMsg.id_proto == 34")
        compare(reservedMsg.import_proto, 35, "reservedMsg.import_proto == 35")
        compare(reservedMsg.property_proto, 36, "reservedMsg.property_proto == 36")
    }

    function test_reservedEnums() {
        compare(MessageEnumReserved.Import, 0, "MessageEnumReserved.Import == 0")
        compare(MessageEnumReserved.Property, 1, "MessageEnumReserved.Property == 1")
        compare(MessageEnumReserved.Id, 2, "MessageEnumReserved.Id == 2")
    }

    function test_caseSense() {
        caseSenseMsg.testField = 34;
        compare(caseSenseMsg.testField, 34, "MessageUpperCase == 34")

        compare(MessageEnumReserved.EnumValue0 == 0, true, "MessageEnumReserved.EnumValue0 == 0")
        compare(MessageEnumReserved.EnumValue1 == 1, true, "MessageEnumReserved.EnumValue1 == 1")
        compare(MessageEnumReserved.EnumValue2 == 2, true, "MessageEnumReserved.EnumValue2 == 2")
    }

    function test_underScoreField() {
        underScoreMsg.underScoreMessageField = 123
        compare(underScoreMsg.underScoreMessageField, 123, "underScoreMsg.underScoreMessageField == 123")
    }

    function test_lowerCaseMessage() {
        lowerCaseMsg.testField = 34
        compare(lowerCaseMsg.testField, 34, "LowerCaseMessageName == 34")
    }

    function test_int32ImplicitConversion() {
        int32Msg.testFieldInt = 0
        compare(int32Msg.testFieldInt ? true : false, false, "Invalid implicit conversion: " + int32Msg.testFieldInt + " should be false")

        int32Msg.testFieldInt = 1
        compare(int32Msg.testFieldInt ? true : false, true, "Invalid implicit conversion: " + int32Msg.testFieldInt + " should be true")
    }

    function test_int32LocaleStringConversion() {
        compare(int32Msg.testFieldInt.toLocaleString(Qt.locale()), Number(int32Msg.testFieldInt).toLocaleString(Qt.locale()),
                "Locale number string is not match " + int32Msg.testFieldInt.toLocaleString(Qt.locale()) + " != " + Number(int32Msg.testFieldInt).toLocaleString(Qt.locale()))
    }

    function test_fixed32ImplicitConversion() {
        fixed32Msg.testFieldFixedInt32 = 0
        compare(fixed32Msg.testFieldFixedInt32 ? true : false, false, "Invalid implicit conversion: " + fixed32Msg.testFieldInt + " should be false")

        fixed32Msg.testFieldFixedInt32 = 1
        compare(fixed32Msg.testFieldFixedInt32 ? true : false, true, "Invalid implicit conversion: " + fixed32Msg.testFieldInt + " should be true")
    }

    function test_fixed32LocaleStringConversion() {
        compare(fixed32Msg.testFieldFixedInt32.toLocaleString(Qt.locale()), Number(fixed32Msg.testFieldFixedInt32).toLocaleString(Qt.locale()),
                "Locale number string is not match " + fixed32Msg.testFieldFixedInt32.toLocaleString(Qt.locale()) + " != " + Number(fixed32Msg.testFieldFixedInt32).toLocaleString(Qt.locale()))
    }

    function test_sint32ImplicitConversion() {
        sint32Msg.testFieldInt = 0
        compare(sint32Msg.testFieldInt ? true : false, false, "Invalid implicit conversion: " + sint32Msg.testFieldInt + " should be false")

        sint32Msg.testFieldInt = 1
        compare(sint32Msg.testFieldInt ? true : false, true, "Invalid implicit conversion: " + sint32Msg.testFieldInt + " should be true")
    }

    function test_sint32LocaleStringConversion() {
        compare(sint32Msg.testFieldInt.toLocaleString(Qt.locale()), Number(sint32Msg.testFieldInt).toLocaleString(Qt.locale()),
                "Locale number string is not match " + sint32Msg.testFieldInt.toLocaleString(Qt.locale()) + " != " + Number(sint32Msg.testFieldInt).toLocaleString(Qt.locale()))
    }

    function test_sfixed32ImplicitConversion() {
        sfixed32Msg.testFieldFixedInt32 = 0
        compare(sfixed32Msg.testFieldFixedInt32 ? true : false, false, "Invalid implicit conversion: " + sfixed32Msg.testFieldInt + " should be false")

        sfixed32Msg.testFieldFixedInt32 = 1
        compare(sfixed32Msg.testFieldFixedInt32 ? true : false, true, "Invalid implicit conversion: " + sfixed32Msg.testFieldInt + " should be true")
    }

    function test_sfixed32LocaleStringConversion() {
        compare(sfixed32Msg.testFieldFixedInt32.toLocaleString(Qt.locale()), Number(sfixed32Msg.testFieldFixedInt32).toLocaleString(Qt.locale()),
                "Locale number string is not match " + sfixed32Msg.testFieldFixedInt32.toLocaleString(Qt.locale()) + " != " + Number(sfixed32Msg.testFieldFixedInt32).toLocaleString(Qt.locale()))
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

    function test_nestedMessage() {
        compare(nestedMsg.testFieldInt, 100, "Nested message initialization failed");
        compare(nestedParent.nested, nestedMsg, "Nested message assignment failed");
    }

    function test_noPackageMessageUser() {
        compare(noPackageMessageUser.testField.testFieldInt, 42, "Package-less message contains invalid value");
        noPackageMessageUser.testField.testFieldInt = 43;
        compare(noPackageMessageUser.testField.testFieldInt, 43, "Package-less message contains invalid value");
    }
}
