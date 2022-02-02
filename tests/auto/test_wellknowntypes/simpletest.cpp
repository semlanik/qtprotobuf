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

#include <QVariantList>
#include <QMetaProperty>
#include <QSignalSpy>
#include <QProtobufSerializer>
#include <QDateTime>

#include <stdio.h>
#include <iostream>
#include <gtest/gtest.h>
#include <google/protobuf/any.qpb.h>
#include <google/protobuf/api.qpb.h>
#include <google/protobuf/type.qpb.h>
#include <google/protobuf/struct.qpb.h>
#include <google/protobuf/empty.qpb.h>
#include <google/protobuf/duration.qpb.h>
#include <google/protobuf/wrappers.qpb.h>
#include <google/protobuf/field_mask.qpb.h>

#include "wellknowntypes.qpb.h"
#include <qtprotobuftestscommon.h>

using namespace google::protobuf;

class WellknowntypesTest : public ::testing::Test
{
public:
    // see simpletest.proto for property names and their field indices
    WellknowntypesTest() {
    }

    static void SetUpTestCase() {
        qRegisterProtobufTypes();
        serializer.reset(new QProtobufSerializer);
    }

    static std::unique_ptr<QProtobufSerializer> serializer;
};

std::unique_ptr<QProtobufSerializer> WellknowntypesTest::serializer;

TEST_F(WellknowntypesTest, AnyTest)
{
    ASSERT_GT(qMetaTypeId<Any>(), 0);
    qProtobufAssertMessagePropertyRegistered<Any, QString>(1, "QString", "typeUrl");
    qProtobufAssertMessagePropertyRegistered<Any, QByteArray>(2, "QByteArray", "value");
}

TEST_F(WellknowntypesTest, ApiTest)
{
    ASSERT_GT(qMetaTypeId<Api>(), 0);

    qProtobufAssertMessagePropertyRegistered<Api, QString>(1, "QString", "name");
    qProtobufAssertMessagePropertyRegistered<Api, MethodRepeated>(2, "google::protobuf::MethodRepeated", "methodsData");
    qProtobufAssertMessagePropertyRegistered<Api, OptionRepeated>(3, "google::protobuf::OptionRepeated", "optionsData");
    qProtobufAssertMessagePropertyRegistered<Api, QString>(4, "QString", "version");
    qProtobufAssertMessagePropertyRegistered<Api, SourceContext>(5, "google::protobuf::SourceContext*", "sourceContext", true);
    qProtobufAssertMessagePropertyRegistered<Api, MixinRepeated>(6, "google::protobuf::MixinRepeated", "mixinsData");
    qProtobufAssertMessagePropertyRegistered<Api, SyntaxGadget::Syntax>(7, "google::protobuf::SyntaxGadget::Syntax", "syntax");
}

TEST_F(WellknowntypesTest, MethodTest)
{
    ASSERT_GT(qMetaTypeId<Method>(), 0);
    qProtobufAssertMessagePropertyRegistered<Method, QString>(1, "QString", "name");
    qProtobufAssertMessagePropertyRegistered<Method, QString>(2, "QString", "requestTypeUrl");
    qProtobufAssertMessagePropertyRegistered<Method, bool>(3, "bool", "requestStreaming");
    qProtobufAssertMessagePropertyRegistered<Method, QString>(4, "QString", "responseTypeUrl");
    qProtobufAssertMessagePropertyRegistered<Method, bool>(5, "bool", "responseStreaming");
    qProtobufAssertMessagePropertyRegistered<Method, OptionRepeated>(6, "google::protobuf::OptionRepeated", "optionsData");
    qProtobufAssertMessagePropertyRegistered<Method, SyntaxGadget::Syntax>(7, "google::protobuf::SyntaxGadget::Syntax", "syntax");
}

TEST_F(WellknowntypesTest, MixinTest)
{
    ASSERT_GT(qMetaTypeId<Mixin>(), 0);
    qProtobufAssertMessagePropertyRegistered<Mixin, QString>(1, "QString", "name");
    qProtobufAssertMessagePropertyRegistered<Mixin, QString>(2, "QString", "root");
}

TEST_F(WellknowntypesTest, DurationTest)
{
    ASSERT_GT(qMetaTypeId<Duration>(), 0);
    qProtobufAssertMessagePropertyRegistered<Duration, QtProtobuf::int64>(1, "QtProtobuf::int64", "seconds");
    qProtobufAssertMessagePropertyRegistered<Duration, QtProtobuf::int32>(2, "QtProtobuf::int32", "nanos" QT_PROTOBUF_PROPERTY_SUFFIX);
}

TEST_F(WellknowntypesTest, EmptyTest)
{
    ASSERT_GT(qMetaTypeId<Empty>(), 0);
}

TEST_F(WellknowntypesTest, FieldMaskTest)
{
    ASSERT_GT(qMetaTypeId<FieldMask>(), 0);
    qProtobufAssertMessagePropertyRegistered<FieldMask, QStringList>(1, "QStringList", "paths");
}

TEST_F(WellknowntypesTest, SourceContextTest)
{
    ASSERT_GT(qMetaTypeId<SourceContext>(), 0);
    qProtobufAssertMessagePropertyRegistered<SourceContext, QString>(1, "QString", "fileName");
}

TEST_F(WellknowntypesTest, StructTest)
{
    ASSERT_GT(qMetaTypeId<Struct>(), 0);
    qProtobufAssertMessagePropertyRegistered<Struct, Struct::FieldsEntry>(1, "google::protobuf::Struct::FieldsEntry", "fields");
}

TEST_F(WellknowntypesTest, ValueTest)
{
    ASSERT_GT(qMetaTypeId<Value>(), 0);
    qProtobufAssertMessagePropertyRegistered<Value, NullValueGadget::NullValue>(1, "google::protobuf::NullValueGadget::NullValue", "nullValue");
    qProtobufAssertMessagePropertyRegistered<Value, double>(2, "double", "numberValue");
    qProtobufAssertMessagePropertyRegistered<Value, QString>(3, "QString", "stringValue");
    qProtobufAssertMessagePropertyRegistered<Value, bool>(4, "bool", "boolValue");
    qProtobufAssertMessagePropertyRegistered<Value, Struct *>(5, "google::protobuf::Struct*", "structValue");
    qProtobufAssertMessagePropertyRegistered<Value, ListValue *>(6, "google::protobuf::ListValue*", "listValue");
}

TEST_F(WellknowntypesTest, ListValueTest)
{
    ASSERT_GT(qMetaTypeId<ListValue>(), 0);
    qProtobufAssertMessagePropertyRegistered<ListValue, ValueRepeated>(1, "google::protobuf::ValueRepeated", "valuesData");
}

TEST_F(WellknowntypesTest, TimestampTest)
{
    ASSERT_GT(qMetaTypeId<Timestamp>(), 0);
    qProtobufAssertMessagePropertyRegistered<Timestamp, QtProtobuf::int64>(1, "QtProtobuf::int64", "seconds");
    qProtobufAssertMessagePropertyRegistered<Timestamp, QtProtobuf::int32>(2, "QtProtobuf::int32", "nanos" QT_PROTOBUF_PROPERTY_SUFFIX);
}

TEST_F(WellknowntypesTest, TypeTest)
{
    ASSERT_GT(qMetaTypeId<Type>(), 0);

    qProtobufAssertMessagePropertyRegistered<Type, QString>(1, "QString", "name");
    qProtobufAssertMessagePropertyRegistered<Type, FieldRepeated>(2, "google::protobuf::FieldRepeated", "fieldsData");
    qProtobufAssertMessagePropertyRegistered<Type, QStringList>(3, "QStringList", "oneofs");
    qProtobufAssertMessagePropertyRegistered<Type, OptionRepeated>(4, "google::protobuf::OptionRepeated", "optionsData");
    qProtobufAssertMessagePropertyRegistered<Type, SourceContext *>(5, "google::protobuf::SourceContext*", "sourceContext", true);
    qProtobufAssertMessagePropertyRegistered<Type, SyntaxGadget::Syntax>(6, "google::protobuf::SyntaxGadget::Syntax", "syntax");
}

TEST_F(WellknowntypesTest, FieldTest)
{
    ASSERT_GT(qMetaTypeId<Field>(), 0);

    qProtobufAssertMessagePropertyRegistered<Field, Field::Kind>(1, "Kind", "kind");
    qProtobufAssertMessagePropertyRegistered<Field, Field::Cardinality>(2, "Cardinality", "cardinality");
    qProtobufAssertMessagePropertyRegistered<Field, QtProtobuf::int32>(3, "QtProtobuf::int32", "number" QT_PROTOBUF_PROPERTY_SUFFIX);
    qProtobufAssertMessagePropertyRegistered<Field, QString>(4, "QString", "name");
    qProtobufAssertMessagePropertyRegistered<Field, QString>(6, "QString", "typeUrl");
    qProtobufAssertMessagePropertyRegistered<Field, QtProtobuf::int32>(7, "QtProtobuf::int32", "oneofIndex" QT_PROTOBUF_PROPERTY_SUFFIX);
    qProtobufAssertMessagePropertyRegistered<Field, bool>(8, "bool", "packed");
    qProtobufAssertMessagePropertyRegistered<Field, OptionRepeated>(9, "google::protobuf::OptionRepeated", "optionsData");
    qProtobufAssertMessagePropertyRegistered<Field, QString>(10, "QString", "jsonName");
    qProtobufAssertMessagePropertyRegistered<Field, QString>(11, "QString", "defaultValue");
}

TEST_F(WellknowntypesTest, EnumTest)
{
    ASSERT_GT(qMetaTypeId<Enum>(), 0);
    qProtobufAssertMessagePropertyRegistered<Enum, QString>(1, "QString", "name");
    qProtobufAssertMessagePropertyRegistered<Enum, EnumValueRepeated>(2, "google::protobuf::EnumValueRepeated", "enumvalueData");
    qProtobufAssertMessagePropertyRegistered<Enum, OptionRepeated>(3, "google::protobuf::OptionRepeated", "optionsData");
    qProtobufAssertMessagePropertyRegistered<Enum, SourceContext *>(4, "google::protobuf::SourceContext*", "sourceContext", true);
    qProtobufAssertMessagePropertyRegistered<Enum, SyntaxGadget::Syntax>(5, "google::protobuf::SyntaxGadget::Syntax", "syntax");
}

TEST_F(WellknowntypesTest, EnumValueTest)
{
    ASSERT_GT(qMetaTypeId<EnumValue>(), 0);
    qProtobufAssertMessagePropertyRegistered<EnumValue, QString>(1, "QString", "name");
    qProtobufAssertMessagePropertyRegistered<EnumValue, QtProtobuf::int32>(2, "QtProtobuf::int32", "number" QT_PROTOBUF_PROPERTY_SUFFIX);
    qProtobufAssertMessagePropertyRegistered<EnumValue, OptionRepeated>(3, "google::protobuf::OptionRepeated", "optionsData");
}

TEST_F(WellknowntypesTest, OptionTest)
{
    ASSERT_GT(qMetaTypeId<Option>(), 0);
    qProtobufAssertMessagePropertyRegistered<Option, QString>(1, "QString", "name");
    qProtobufAssertMessagePropertyRegistered<Option, Any *>(2, "google::protobuf::Any*", "value", true);
}

TEST_F(WellknowntypesTest, DoubleValueTest)
{
    ASSERT_GT(qMetaTypeId<DoubleValue>(), 0);
    qProtobufAssertMessagePropertyRegistered<DoubleValue, double>(1, "double", "value");
}

TEST_F(WellknowntypesTest, FloatValueTest)
{
    ASSERT_GT(qMetaTypeId<FloatValue>(), 0);
    qProtobufAssertMessagePropertyRegistered<FloatValue, float>(1, "float", "value");
}

TEST_F(WellknowntypesTest, Int64ValueTest)
{
    ASSERT_GT(qMetaTypeId<Int64Value>(), 0);
    qProtobufAssertMessagePropertyRegistered<Int64Value, QtProtobuf::int64>(1, "QtProtobuf::int64", "value");
}

TEST_F(WellknowntypesTest, UInt64ValueTest)
{
    ASSERT_GT(qMetaTypeId<UInt64Value>(), 0);
    qProtobufAssertMessagePropertyRegistered<UInt64Value, QtProtobuf::uint64>(1, "QtProtobuf::uint64", "value");
}

TEST_F(WellknowntypesTest, Int32ValueTest)
{
    ASSERT_GT(qMetaTypeId<Int32Value>(), 0);
    qProtobufAssertMessagePropertyRegistered<Int32Value, QtProtobuf::int32>(1, "QtProtobuf::int32", "value" QT_PROTOBUF_PROPERTY_SUFFIX);
}

TEST_F(WellknowntypesTest, UInt32ValueTest)
{
    ASSERT_GT(qMetaTypeId<UInt32Value>(), 0);
    qProtobufAssertMessagePropertyRegistered<UInt32Value, QtProtobuf::uint32>(1, "QtProtobuf::uint32", "value");
}

TEST_F(WellknowntypesTest, StringValueTest)
{
    ASSERT_GT(qMetaTypeId<StringValue>(), 0);
    qProtobufAssertMessagePropertyRegistered<StringValue, QString>(1, "QString", "value");
}

TEST_F(WellknowntypesTest, BytesValueTest)
{
    ASSERT_GT(qMetaTypeId<BytesValue>(), 0);
    qProtobufAssertMessagePropertyRegistered<BytesValue, QByteArray>(1, "QByteArray", "value");
}

TEST_F(WellknowntypesTest, TimestampMessageTest)
{
    qProtobufAssertMessagePropertyRegistered<qtprotobufnamespace::wellknowntypes::tests::TimestampMessage, google::protobuf::Timestamp*>(1, "google::protobuf::Timestamp*", "testField");
}

TEST_F(WellknowntypesTest, TimestampMessageSerializationTest) {
    qtprotobufnamespace::wellknowntypes::tests::TimestampMessage msg;

    QDateTime deserializedDateTime;
    QDateTime originalDateTime = QDateTime::currentDateTime();

    msg.setTestField({originalDateTime, nullptr});
    ASSERT_EQ(msg.testField().seconds(), originalDateTime.toMSecsSinceEpoch() / 1000);
    ASSERT_EQ(msg.testField().nanos(), (originalDateTime.toMSecsSinceEpoch() % 1000) * 1000);

    QByteArray val = msg.serialize(serializer.get());
    msg.setTestField({0, 0, nullptr});

    ASSERT_EQ(msg.testField().seconds(), 0);
    ASSERT_EQ(msg.testField().nanos(), 0);

    msg.deserialize(serializer.get(), val);
    deserializedDateTime = msg.testField();

    ASSERT_EQ(deserializedDateTime.toMSecsSinceEpoch(), originalDateTime.toMSecsSinceEpoch());
    ASSERT_TRUE(deserializedDateTime == originalDateTime);
}
