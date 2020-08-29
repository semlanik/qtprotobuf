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
#include "../testscommon.h"

using namespace google::protobuf;

namespace QtProtobuf {
namespace tests {

class WellknowntypesTest : public ::testing::Test
{
public:
    // see simpletest.proto for property names and their field indices
    WellknowntypesTest() {
    }

    static void SetUpTestCase() {
        QtProtobuf::qRegisterProtobufTypes();
        serializer.reset(new QProtobufSerializer);
    }

    static std::unique_ptr<QProtobufSerializer> serializer;
};

std::unique_ptr<QProtobufSerializer> WellknowntypesTest::serializer;

TEST_F(WellknowntypesTest, AnyTest)
{
    ASSERT_GT(qMetaTypeId<Any>(), 0);
    assertMessagePropertyRegistered<Any, QString>(1, "QString", "typeUrl");
    assertMessagePropertyRegistered<Any, QByteArray>(2, "QByteArray", "value");
}

TEST_F(WellknowntypesTest, ApiTest)
{
    ASSERT_GT(qMetaTypeId<Api>(), 0);

    assertMessagePropertyRegistered<Api, QString>(1, "QString", "name");
    assertMessagePropertyRegistered<Api, MethodRepeated>(2, "google::protobuf::MethodRepeated", "methods");
    assertMessagePropertyRegistered<Api, OptionRepeated>(3, "google::protobuf::OptionRepeated", "options");
    assertMessagePropertyRegistered<Api, QString>(4, "QString", "version");
    assertMessagePropertyRegistered<Api, SourceContext>(5, "google::protobuf::SourceContext*", "sourceContext", true);
    assertMessagePropertyRegistered<Api, MixinRepeated>(6, "google::protobuf::MixinRepeated", "mixins");
    assertMessagePropertyRegistered<Api, SyntaxGadget::Syntax>(7, "google::protobuf::SyntaxGadget::Syntax", "syntax");
}

TEST_F(WellknowntypesTest, MethodTest)
{
    ASSERT_GT(qMetaTypeId<Method>(), 0);
    assertMessagePropertyRegistered<Method, QString>(1, "QString", "name");
    assertMessagePropertyRegistered<Method, QString>(2, "QString", "requestTypeUrl");
    assertMessagePropertyRegistered<Method, bool>(3, "bool", "requestStreaming");
    assertMessagePropertyRegistered<Method, QString>(4, "QString", "responseTypeUrl");
    assertMessagePropertyRegistered<Method, bool>(5, "bool", "responseStreaming");
    assertMessagePropertyRegistered<Method, OptionRepeated>(6, "google::protobuf::OptionRepeated", "options");
    assertMessagePropertyRegistered<Method, SyntaxGadget::Syntax>(7, "google::protobuf::SyntaxGadget::Syntax", "syntax");
}

TEST_F(WellknowntypesTest, MixinTest)
{
    ASSERT_GT(qMetaTypeId<Mixin>(), 0);
    assertMessagePropertyRegistered<Mixin, QString>(1, "QString", "name");
    assertMessagePropertyRegistered<Mixin, QString>(2, "QString", "root");
}

TEST_F(WellknowntypesTest, DurationTest)
{
    ASSERT_GT(qMetaTypeId<Duration>(), 0);
    assertMessagePropertyRegistered<Duration, QtProtobuf::int64>(1, "QtProtobuf::int64", "seconds");
    assertMessagePropertyRegistered<Duration, QtProtobuf::int32>(2, "QtProtobuf::int32", "nanos_p");
}

TEST_F(WellknowntypesTest, EmptyTest)
{
    ASSERT_GT(qMetaTypeId<Empty>(), 0);
}

TEST_F(WellknowntypesTest, FieldMaskTest)
{
    ASSERT_GT(qMetaTypeId<FieldMask>(), 0);
    assertMessagePropertyRegistered<FieldMask, QStringList>(1, "QStringList", "paths");
}

TEST_F(WellknowntypesTest, SourceContextTest)
{
    ASSERT_GT(qMetaTypeId<SourceContext>(), 0);
    assertMessagePropertyRegistered<SourceContext, QString>(1, "QString", "fileName");
}

TEST_F(WellknowntypesTest, StructTest)
{
    ASSERT_GT(qMetaTypeId<Struct>(), 0);
    assertMessagePropertyRegistered<Struct, Struct::FieldsEntry>(1, "google::protobuf::Struct::FieldsEntry", "fields");
}

TEST_F(WellknowntypesTest, ValueTest)
{
    ASSERT_GT(qMetaTypeId<Value>(), 0);
    assertMessagePropertyRegistered<Value, NullValueGadget::NullValue>(1, "google::protobuf::NullValueGadget::NullValue", "nullValue");
    assertMessagePropertyRegistered<Value, double>(2, "double", "numberValue");
    assertMessagePropertyRegistered<Value, QString>(3, "QString", "stringValue");
    assertMessagePropertyRegistered<Value, bool>(4, "bool", "boolValue");
    assertMessagePropertyRegistered<Value, Struct *>(5, "google::protobuf::Struct*", "structValue");
    assertMessagePropertyRegistered<Value, ListValue *>(6, "google::protobuf::ListValue*", "listValue");
}

TEST_F(WellknowntypesTest, ListValueTest)
{
    ASSERT_GT(qMetaTypeId<ListValue>(), 0);
    assertMessagePropertyRegistered<ListValue, ValueRepeated>(1, "google::protobuf::ValueRepeated", "values");
}

TEST_F(WellknowntypesTest, TimestampTest)
{
    ASSERT_GT(qMetaTypeId<Timestamp>(), 0);
    assertMessagePropertyRegistered<Timestamp, QtProtobuf::int64>(1, "QtProtobuf::int64", "seconds");
    assertMessagePropertyRegistered<Timestamp, QtProtobuf::int32>(2, "QtProtobuf::int32", "nanos_p");
}

TEST_F(WellknowntypesTest, TypeTest)
{
    ASSERT_GT(qMetaTypeId<Type>(), 0);
    Q_PROPERTY(QStringList oneofs READ oneofs WRITE setOneofs NOTIFY oneofsChanged)

    assertMessagePropertyRegistered<Type, QString>(1, "QString", "name");
    assertMessagePropertyRegistered<Type, FieldRepeated>(2, "google::protobuf::FieldRepeated", "fields");
    assertMessagePropertyRegistered<Type, QStringList>(3, "QStringList", "oneofs");
    assertMessagePropertyRegistered<Type, OptionRepeated>(4, "google::protobuf::OptionRepeated", "options");
    assertMessagePropertyRegistered<Type, SourceContext *>(5, "google::protobuf::SourceContext*", "sourceContext", true);
    assertMessagePropertyRegistered<Type, SyntaxGadget::Syntax>(6, "google::protobuf::SyntaxGadget::Syntax", "syntax");
}

TEST_F(WellknowntypesTest, FieldTest)
{
    ASSERT_GT(qMetaTypeId<Field>(), 0);

    assertMessagePropertyRegistered<Field, Field::Kind>(1, "Kind", "kind");
    assertMessagePropertyRegistered<Field, Field::Cardinality>(2, "Cardinality", "cardinality");
    assertMessagePropertyRegistered<Field, QtProtobuf::int32>(3, "QtProtobuf::int32", "number_p");
    assertMessagePropertyRegistered<Field, QString>(4, "QString", "name");
    assertMessagePropertyRegistered<Field, QString>(6, "QString", "typeUrl");
    assertMessagePropertyRegistered<Field, QtProtobuf::int32>(7, "QtProtobuf::int32", "oneofIndex_p");
    assertMessagePropertyRegistered<Field, bool>(8, "bool", "packed");
    assertMessagePropertyRegistered<Field, OptionRepeated>(9, "google::protobuf::OptionRepeated", "options");
    assertMessagePropertyRegistered<Field, QString>(10, "QString", "jsonName");
    assertMessagePropertyRegistered<Field, QString>(11, "QString", "defaultValue");
}

TEST_F(WellknowntypesTest, EnumTest)
{
    ASSERT_GT(qMetaTypeId<Enum>(), 0);
    assertMessagePropertyRegistered<Enum, QString>(1, "QString", "name");
    assertMessagePropertyRegistered<Enum, EnumValueRepeated>(2, "google::protobuf::EnumValueRepeated", "enumvalue");
    assertMessagePropertyRegistered<Enum, OptionRepeated>(3, "google::protobuf::OptionRepeated", "options");
    assertMessagePropertyRegistered<Enum, SourceContext *>(4, "google::protobuf::SourceContext*", "sourceContext", true);
    assertMessagePropertyRegistered<Enum, SyntaxGadget::Syntax>(5, "google::protobuf::SyntaxGadget::Syntax", "syntax");
}

TEST_F(WellknowntypesTest, EnumValueTest)
{
    ASSERT_GT(qMetaTypeId<EnumValue>(), 0);
    assertMessagePropertyRegistered<EnumValue, QString>(1, "QString", "name");
    assertMessagePropertyRegistered<EnumValue, QtProtobuf::int32>(2, "QtProtobuf::int32", "number_p");
    assertMessagePropertyRegistered<EnumValue, OptionRepeated>(3, "google::protobuf::OptionRepeated", "options");
}

TEST_F(WellknowntypesTest, OptionTest)
{
    ASSERT_GT(qMetaTypeId<Option>(), 0);
    assertMessagePropertyRegistered<Option, QString>(1, "QString", "name");
    assertMessagePropertyRegistered<Option, Any *>(2, "google::protobuf::Any*", "value", true);
}

TEST_F(WellknowntypesTest, DoubleValueTest)
{
    ASSERT_GT(qMetaTypeId<DoubleValue>(), 0);
    assertMessagePropertyRegistered<DoubleValue, double>(1, "double", "value");
}

TEST_F(WellknowntypesTest, FloatValueTest)
{
    ASSERT_GT(qMetaTypeId<FloatValue>(), 0);
    assertMessagePropertyRegistered<FloatValue, float>(1, "float", "value");
}

TEST_F(WellknowntypesTest, Int64ValueTest)
{
    ASSERT_GT(qMetaTypeId<Int64Value>(), 0);
    assertMessagePropertyRegistered<Int64Value, QtProtobuf::int64>(1, "QtProtobuf::int64", "value");
}

TEST_F(WellknowntypesTest, UInt64ValueTest)
{
    ASSERT_GT(qMetaTypeId<UInt64Value>(), 0);
    assertMessagePropertyRegistered<UInt64Value, QtProtobuf::uint64>(1, "QtProtobuf::uint64", "value");
}

TEST_F(WellknowntypesTest, Int32ValueTest)
{
    ASSERT_GT(qMetaTypeId<Int32Value>(), 0);
    assertMessagePropertyRegistered<Int32Value, QtProtobuf::int32>(1, "QtProtobuf::int32", "value_p");
}

TEST_F(WellknowntypesTest, UInt32ValueTest)
{
    ASSERT_GT(qMetaTypeId<UInt32Value>(), 0);
    assertMessagePropertyRegistered<UInt32Value, QtProtobuf::uint32>(1, "QtProtobuf::uint32", "value");
}

TEST_F(WellknowntypesTest, StringValueTest)
{
    ASSERT_GT(qMetaTypeId<StringValue>(), 0);
    assertMessagePropertyRegistered<StringValue, QString>(1, "QString", "value");
}

TEST_F(WellknowntypesTest, BytesValueTest)
{
    ASSERT_GT(qMetaTypeId<BytesValue>(), 0);
    assertMessagePropertyRegistered<BytesValue, QByteArray>(1, "QByteArray", "value");
}

TEST_F(WellknowntypesTest, TimestampMessageTest)
{
    assertMessagePropertyRegistered<qtprotobufnamespace::wellknowntypes::tests::TimestampMessage, google::protobuf::Timestamp*>(1, "google::protobuf::Timestamp*", "testField");
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
}
}
