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

import QtProtobuf 0.1
import qtprotobufnamespace.tests 1.0

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

    function test_simpleboolmessage() {
        boolMsg.testFieldBool = true;
        compare(boolMsg.testFieldBool, true, "SimpleBoolMessage == true")
        boolMsg.testFieldBool = false;
        compare(boolMsg.testFieldBool, false, "SimpleBoolMessage == false")
    }

    function test_simpleintmessage() {
        compare(int32Msg.testFieldInt == 2147483647, true, "SimpleIntMessage.testFieldInt: 2147483647: " + int32Msg.testFieldInt)
        int32Msg.testFieldInt = 0;
        compare(int32Msg.testFieldInt == 0, true, "SimpleIntMessage == 0")
        int32Msg.testFieldInt = -128;
        compare(int32Msg.testFieldInt == -128, true, "SimpleIntMessage == -128")
        int32Msg.testFieldInt = 127;
        compare(int32Msg.testFieldInt == 127, true, "SimpleIntMessage == 127")
        int32Msg.testFieldInt = -256;
        compare(int32Msg.testFieldInt == -256, true, "SimpleIntMessage == -256")
        int32Msg.testFieldInt = 255;
        compare(int32Msg.testFieldInt == 255, true, "SimpleIntMessage == 255")
        int32Msg.testFieldInt = -32768;
        compare(int32Msg.testFieldInt == -32768, true, "SimpleIntMessage == -32768")
        int32Msg.testFieldInt = 32767;
        compare(int32Msg.testFieldInt == 32767, true, "SimpleIntMessage == 32767")
        int32Msg.testFieldInt = -65536;
        compare(int32Msg.testFieldInt == -65536, true, "SimpleIntMessage == -65536")
        int32Msg.testFieldInt = 65535;
        compare(int32Msg.testFieldInt == 65535, true, "SimpleIntMessage == 65535")
        int32Msg.testFieldInt = -2147483648;
        compare(int32Msg.testFieldInt == -2147483648, true, "SimpleIntMessage == -2147483648")
        int32Msg.testFieldInt = 2147483647;
        compare(int32Msg.testFieldInt == 2147483647, true, "SimpleIntMessage == 2147483647")
    }

    function test_simplesintmessage() {
        compare(sint32Msg.testFieldInt == 2147483647, true, "SimpleSIntMessage.testFieldInt: 2147483647: " + sint32Msg.testFieldInt)
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
        compare(uint32Msg.testFieldInt == 4294967295, true, "SimpleUIntMessage.testFieldInt: 4294967295: " + uint32Msg.testFieldInt)
        uint32Msg.testFieldInt = 0;
        compare(uint32Msg.testFieldInt == 0, true, "SimpleUIntMessage == 0")
        uint32Msg.testFieldInt = 127;
        compare(uint32Msg.testFieldInt == 127, true, "SimpleUIntMessage == 127")
        uint32Msg.testFieldInt = 255;
        compare(uint32Msg.testFieldInt == 255, true, "SimpleUIntMessage == 255")
        uint32Msg.testFieldInt = 32767;
        compare(uint32Msg.testFieldInt == 32767, true, "SimpleUIntMessage == 32767")
        uint32Msg.testFieldInt = 65535;
        compare(uint32Msg.testFieldInt == 65535, true, "SimpleUIntMessage == 65535")
        uint32Msg.testFieldInt = 2147483647;
        compare(uint32Msg.testFieldInt == 2147483647, true, "SimpleUIntMessage == 2147483647")
        uint32Msg.testFieldInt = 4294967295;
        compare(uint32Msg.testFieldInt == 4294967295, true, "SimpleUIntMessage == 4294967295")
    }

    function test_simplefixed32message() {
        compare(fixed32Msg.testFieldFixedInt32 == 4294967295, true, "SimpleFixedInt32Message.testFieldInt: 4294967295: " + fixed32Msg.testFieldFixedInt32)
        fixed32Msg.testFieldFixedInt32 = 0;
        compare(fixed32Msg.testFieldFixedInt32 == 0, true, "SimpleFixedInt32Message == 0")
        fixed32Msg.testFieldFixedInt32 = 127;
        compare(fixed32Msg.testFieldFixedInt32 == 127, true, "SimpleFixedInt32Message == 127")
        fixed32Msg.testFieldFixedInt32 = 255;
        compare(fixed32Msg.testFieldFixedInt32 == 255, true, "SimpleFixedInt32Message == 255")
        fixed32Msg.testFieldFixedInt32 = 32767;
        compare(fixed32Msg.testFieldFixedInt32 == 32767, true, "SimpleFixedInt32Message == 32767")
        fixed32Msg.testFieldFixedInt32 = 65535;
        compare(fixed32Msg.testFieldFixedInt32 == 65535, true, "SimpleFixedInt32Message == 65535")
        fixed32Msg.testFieldFixedInt32 = 2147483647;
        compare(fixed32Msg.testFieldFixedInt32 == 2147483647, true, "SimpleFixedInt32Message == 2147483647")
        fixed32Msg.testFieldFixedInt32 = 4294967295;
        compare(fixed32Msg.testFieldFixedInt32 == 4294967295, true, "SimpleFixedInt32Message == 4294967295")
    }

    function test_simplesfixed32message() {
        compare(sfixed32Msg.testFieldFixedInt32 == 2147483647, true, "SimpleSFixedInt32Message.testFieldInt: 2147483647: " + sfixed32Msg.testFieldFixedInt32)
        sfixed32Msg.testFieldFixedInt32 = 0;
        compare(sfixed32Msg.testFieldFixedInt32 == 0, true, "SimpleSFixedInt32Message == 0")
        sfixed32Msg.testFieldFixedInt32 = -128;
        compare(sfixed32Msg.testFieldFixedInt32 == -128, true, "SimpleSFixedInt32Message == -128")
        sfixed32Msg.testFieldFixedInt32 = 127;
        compare(sfixed32Msg.testFieldFixedInt32 == 127, true, "SimpleSFixedInt32Message == 127")
        sfixed32Msg.testFieldFixedInt32 = -256;
        compare(sfixed32Msg.testFieldFixedInt32 == -256, true, "SimpleSFixedInt32Message == -256")
        sfixed32Msg.testFieldFixedInt32 = 255;
        compare(sfixed32Msg.testFieldFixedInt32 == 255, true, "SimpleSFixedInt32Message == 255")
        sfixed32Msg.testFieldFixedInt32 = -32768;
        compare(sfixed32Msg.testFieldFixedInt32 == -32768, true, "SimpleSFixedInt32Message == -32768")
        sfixed32Msg.testFieldFixedInt32 = 32767;
        compare(sfixed32Msg.testFieldFixedInt32 == 32767, true, "SimpleSFixedInt32Message == 32767")
        sfixed32Msg.testFieldFixedInt32 = -65536;
        compare(sfixed32Msg.testFieldFixedInt32 == -65536, true, "SimpleSFixedInt32Message == -65536")
        sfixed32Msg.testFieldFixedInt32 = 65535;
        compare(sfixed32Msg.testFieldFixedInt32 == 65535, true, "SimpleSFixedInt32Message == 65535")
        sfixed32Msg.testFieldFixedInt32 = -2147483648;
        compare(sfixed32Msg.testFieldFixedInt32 == -2147483648, true, "SimpleSFixedInt32Message == -2147483648")
        sfixed32Msg.testFieldFixedInt32 = 2147483647;
        compare(sfixed32Msg.testFieldFixedInt32 == 2147483647, true, "SimpleSFixedInt32Message == 2147483647")
    }

    function test_simplesstringmessage() {
        compare(stringMsg.testFieldString, "Test string", "SimpleStringMessage")
    }

    function test_reservedNames() {
        reservedMsg.idProto = 34;
        reservedMsg.importProto = 35;
        reservedMsg.propertyProto = 36;
        compare(reservedMsg.idProto, 34, "reservedMsg.idProto == 34")
        compare(reservedMsg.importProto, 35, "reservedMsg.importProto == 35")
        compare(reservedMsg.propertyProto, 36, "reservedMsg.propertyProto == 36")
    }

    function test_reservedEnums() {
        compare(MessageEnumReserved.Import, 0, "MessageEnumReserved.Import == 0")
        compare(MessageEnumReserved.Property, 1, "MessageEnumReserved.Property == 1")
        compare(MessageEnumReserved.Id, 2, "MessageEnumReserved.Id == 2")
    }

    function test_caseSense() {
        caseSenseMsg.testField = 34;
        compare(caseSenseMsg.testField == 34, true, "MessageUpperCase == 34")

        compare(MessageEnumReserved.EnumValue0 == 0, true, "MessageEnumReserved.EnumValue0 == 0")
        compare(MessageEnumReserved.EnumValue1 == 1, true, "MessageEnumReserved.EnumValue1 == 1")
        compare(MessageEnumReserved.EnumValue2 == 2, true, "MessageEnumReserved.EnumValue2 == 2")
    }

    function test_underScoreField() {
        underScoreMsg._underScoreMessageField = 123
        compare(underScoreMsg._underScoreMessageField == 123, true, "underScoreMsg._underScoreMessageField == 123")
    }

    function test_lowerCaseMessage() {
        lowerCaseMsg.testField = 34
        compare(lowerCaseMsg.testField == 34, true, "LowerCaseMessageName == 34")
    }
}
