/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>, Viktor Kopp <vifactor@gmail.com>
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

#include <gtest/gtest.h>
#include "../testscommon.h"

using namespace qtprotobufnamespace::tests;

namespace QtProtobuf {
namespace tests {

class SimpleTest : public ::testing::Test
{
public:
    // see simpletest.proto for property names and their field indices
    SimpleTest()
    {
    }

    static void SetUpTestCase();
};

void SimpleTest::SetUpTestCase()
{
    QtProtobuf::qRegisterProtobufTypes();
}

TEST_F(SimpleTest, SimpleBoolMessageTest)
{
    const char *propertyName = "testFieldBool";
    assertMessagePropertyRegistered<SimpleBoolMessage, bool>(1, "bool", propertyName);

    SimpleBoolMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue(true)));
    ASSERT_EQ(test.property(propertyName).value<bool>(), true);
    ASSERT_EQ(test.testFieldBool(), true);

    EXPECT_EQ(SimpleBoolMessage::TestFieldBoolProtoFieldNumber, 1);
}

TEST_F(SimpleTest, SimpleIntMessageTest)
{
    const char *propertyName = "testFieldInt_p";
    assertMessagePropertyRegistered<qtprotobufnamespace::tests::SimpleIntMessage, int32>(1, "QtProtobuf::int32", propertyName);

    qtprotobufnamespace::tests::SimpleIntMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<int32>(1)));
    ASSERT_EQ(test.property(propertyName).value<int32>(), 1);
    ASSERT_EQ(test.testFieldInt(), 1);

    EXPECT_EQ(qtprotobufnamespace::tests::SimpleIntMessage::TestFieldIntProtoFieldNumber, 1);
}

TEST_F(SimpleTest, SimpleSIntMessageTest)
{
    const char *propertyName = "testFieldInt";
    assertMessagePropertyRegistered<SimpleSIntMessage, sint32>(1, "QtProtobuf::sint32", propertyName);

    SimpleSIntMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<sint32>(1)));
    ASSERT_EQ(test.property(propertyName).value<sint32>(), 1);
    ASSERT_EQ(test.testFieldInt(), 1);

    EXPECT_EQ(SimpleSIntMessage::TestFieldIntProtoFieldNumber, 1);
}

TEST_F(SimpleTest, SimpleUIntMessageTest)
{
    const char *propertyName = "testFieldInt";
    assertMessagePropertyRegistered<SimpleUIntMessage, uint32>(1, "QtProtobuf::uint32", propertyName);

    SimpleUIntMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<uint32>(1)));
    ASSERT_EQ(test.property(propertyName).value<uint32>(), 1);
    ASSERT_EQ(test.testFieldInt(), 1);

    EXPECT_EQ(SimpleUIntMessage::TestFieldIntProtoFieldNumber, 1);
}

TEST_F(SimpleTest, SimpleInt64MessageTest)
{
    const char *propertyName = "testFieldInt";
    assertMessagePropertyRegistered<SimpleInt64Message, int64>(1, "QtProtobuf::int64", propertyName);

    SimpleInt64Message test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<int64>(1)));
    ASSERT_EQ(test.property(propertyName).value<int64>(), 1);
    ASSERT_EQ(test.testFieldInt(), 1);

    EXPECT_EQ(SimpleInt64Message::TestFieldIntProtoFieldNumber, 1);
}

TEST_F(SimpleTest, SimpleSInt64MessageTest)
{
    const char *propertyName = "testFieldInt";
    assertMessagePropertyRegistered<SimpleSInt64Message, sint64>(1, "QtProtobuf::sint64", propertyName);

    SimpleSInt64Message test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<sint64>(1)));
    ASSERT_EQ(test.property(propertyName).value<sint64>(), 1);
    ASSERT_EQ(test.testFieldInt(), 1);

    EXPECT_EQ(SimpleSInt64Message::TestFieldIntProtoFieldNumber, 1);
}

TEST_F(SimpleTest, SimpleUInt64MessageTest)
{
    const char *propertyName = "testFieldInt";
    assertMessagePropertyRegistered<SimpleUInt64Message, uint64>(1, "QtProtobuf::uint64", propertyName);

    SimpleUInt64Message test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<uint64>(1)));
    ASSERT_EQ(test.property(propertyName).value<uint64>(), 1);
    ASSERT_EQ(test.testFieldInt(), 1);

    EXPECT_EQ(SimpleUInt64Message::TestFieldIntProtoFieldNumber, 1);
}

TEST_F(SimpleTest, SimpleFixedInt32MessageTest)
{
    const char *propertyName = "testFieldFixedInt32_p";
    assertMessagePropertyRegistered<SimpleFixedInt32Message, fixed32>(1, "QtProtobuf::fixed32", propertyName);

    SimpleFixedInt32Message test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<fixed32>(1)));
    ASSERT_EQ(test.property(propertyName).value<fixed32>(), 1);
    ASSERT_EQ(test.testFieldFixedInt32(), 1);

    EXPECT_EQ(SimpleFixedInt32Message::TestFieldFixedInt32ProtoFieldNumber, 1);
}

TEST_F(SimpleTest, SimpleFixedInt64MessageTest)
{
    const char *propertyName = "testFieldFixedInt64";
    assertMessagePropertyRegistered<SimpleFixedInt64Message, fixed64>(1, "QtProtobuf::fixed64", propertyName);

    SimpleFixedInt64Message test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<fixed64>(1)));
    ASSERT_EQ(test.property(propertyName).value<fixed64>(), 1);
    ASSERT_EQ(test.testFieldFixedInt64(), 1);

    EXPECT_EQ(SimpleFixedInt64Message::TestFieldFixedInt64ProtoFieldNumber, 1);
}

TEST_F(SimpleTest, SimpleSFixedInt32MessageTest)
{
    const char *propertyName = "testFieldFixedInt32_p";
    assertMessagePropertyRegistered<SimpleSFixedInt32Message, sfixed32>(1, "QtProtobuf::sfixed32", propertyName);

    SimpleSFixedInt32Message test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<sfixed32>(1)));
    ASSERT_EQ(test.property(propertyName).value<sfixed32>(), 1);
    ASSERT_EQ(test.testFieldFixedInt32(), 1);

    EXPECT_EQ(SimpleSFixedInt32Message::TestFieldFixedInt32ProtoFieldNumber, 1);
}

TEST_F(SimpleTest, SimpleSFixedInt64MessageTest)
{
    const char *propertyName = "testFieldFixedInt64";
    assertMessagePropertyRegistered<SimpleSFixedInt64Message, sfixed64>(1, "QtProtobuf::sfixed64", propertyName);

    SimpleSFixedInt64Message test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<sfixed64>(1)));
    ASSERT_EQ(test.property(propertyName).value<sfixed64>(), 1);
    ASSERT_EQ(test.testFieldFixedInt64(), 1);

    EXPECT_EQ(SimpleSFixedInt64Message::TestFieldFixedInt64ProtoFieldNumber, 1);
}

TEST_F(SimpleTest, SimpleStringMessageTest)
{
    const char *propertyName = "testFieldString";
    SimpleStringMessage test;
    int propertyNumber = SimpleStringMessage::propertyOrdering.at(6); //See simpletest.proto
    ASSERT_EQ(SimpleStringMessage::staticMetaObject.property(propertyNumber).type(), QMetaType::QString);
    ASSERT_STREQ(SimpleStringMessage::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue(QString("test1"))));
    ASSERT_STREQ(test.property(propertyName).toString().toStdString().c_str(), "test1");
    ASSERT_STREQ(test.testFieldString().toStdString().c_str(), "test1");

    EXPECT_EQ(SimpleStringMessage::TestFieldStringProtoFieldNumber, 6);
}

TEST_F(SimpleTest, SimpleFloatMessageTest)
{
    const char *propertyName = "testFieldFloat";
    SimpleFloatMessage test;
    int propertyNumber = SimpleFloatMessage::propertyOrdering.at(7); //See simpletest.proto
    ASSERT_EQ(SimpleFloatMessage::staticMetaObject.property(propertyNumber).type(), QMetaType::Float);
    ASSERT_STREQ(SimpleFloatMessage::staticMetaObject.property(propertyNumber).name(), "testFieldFloat");

    float assignedValue = 1.55f;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<float>(assignedValue)));
    ASSERT_FLOAT_EQ(test.property(propertyName).toFloat(), assignedValue);
    ASSERT_FLOAT_EQ(test.testFieldFloat(), assignedValue);

    EXPECT_EQ(SimpleFloatMessage::TestFieldFloatProtoFieldNumber, 7);
}

TEST_F(SimpleTest, SimpleDoubleMessageTest)
{
    const char *propertyName = "testFieldDouble";
    SimpleDoubleMessage test;
    int propertyNumber = SimpleDoubleMessage::propertyOrdering.at(8); //See simpletest.proto
    ASSERT_EQ(SimpleDoubleMessage::staticMetaObject.property(propertyNumber).type(), QMetaType::Double);
    ASSERT_STREQ(SimpleDoubleMessage::staticMetaObject.property(propertyNumber).name(), propertyName);

    double assignedValue = 0.55;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<double>(assignedValue)));
    ASSERT_DOUBLE_EQ(test.property(propertyName).toDouble(), assignedValue);
    ASSERT_DOUBLE_EQ(test.testFieldDouble(), assignedValue);

    EXPECT_EQ(SimpleDoubleMessage::TestFieldDoubleProtoFieldNumber, 8);
}

TEST_F(SimpleTest, SimpleLocalEnumTest)
{
    ASSERT_GT(SimpleEnumMessage::staticMetaObject.enumeratorCount(), 0);
    QMetaEnum simpleEnum;
    for (int i = 0; i < SimpleEnumMessage::staticMetaObject.enumeratorCount(); i++) {
        QMetaEnum tmp = SimpleEnumMessage::staticMetaObject.enumerator(i);
        if (QString(tmp.name()) == QString("LocalEnum")) {
            simpleEnum = tmp;
            break;
        }
    }
    ASSERT_TRUE(simpleEnum.isValid());
    ASSERT_STREQ(simpleEnum.key(0), "LOCAL_ENUM_VALUE0");
    ASSERT_STREQ(simpleEnum.key(1), "LOCAL_ENUM_VALUE1");
    ASSERT_STREQ(simpleEnum.key(2), "LOCAL_ENUM_VALUE2");
    ASSERT_STREQ(simpleEnum.key(3), "LOCAL_ENUM_VALUE3");

    ASSERT_EQ(simpleEnum.value(0), 0);
    ASSERT_EQ(simpleEnum.value(1), 1);
    ASSERT_EQ(simpleEnum.value(2), 2);
    ASSERT_EQ(simpleEnum.value(3), 3);
}

TEST_F(SimpleTest, SimpleLocalEnumListTest)
{
    ASSERT_GT(SimpleEnumListMessage::staticMetaObject.enumeratorCount(), 0);

    const char *propertyName = "localEnumListData";
    assertMessagePropertyRegistered<SimpleEnumListMessage, SimpleEnumListMessage::LocalEnumRepeated>(1,  "qtprotobufnamespace::tests::SimpleEnumListMessage::LocalEnumRepeated", propertyName);

    SimpleEnumListMessage::LocalEnumRepeated value({SimpleEnumListMessage::LOCAL_ENUM_VALUE2,
                                                SimpleEnumListMessage::LOCAL_ENUM_VALUE2,
                                                SimpleEnumListMessage::LOCAL_ENUM_VALUE1,
                                                SimpleEnumListMessage::LOCAL_ENUM_VALUE3});

    SimpleEnumListMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<SimpleEnumListMessage::LocalEnumRepeated>(value)));
    ASSERT_TRUE(test.property(propertyName).value<SimpleEnumListMessage::LocalEnumRepeated>() == value);
    ASSERT_TRUE(test.localEnumList() == value);
}

TEST_F(SimpleTest, SimpleExternalEnumMessageTest)
{
    using ExternalGlobalEnums = qtprotobufnamespace1::externaltests::ExternalTestEnumGadget;

    const char *propertyName = "externalEnum";
    assertMessagePropertyRegistered<SimpleExternalEnumMessage, ExternalGlobalEnums::ExternalTestEnum>(1, "qtprotobufnamespace1::externaltests::ExternalTestEnumGadget::ExternalTestEnum", propertyName);

    SimpleExternalEnumMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<ExternalGlobalEnums::ExternalTestEnum>(ExternalGlobalEnums::EXTERNAL_TEST_ENUM_VALUE4)));
    ASSERT_TRUE(test.property(propertyName).value<ExternalGlobalEnums::ExternalTestEnum>() == QVariant::fromValue<ExternalGlobalEnums::ExternalTestEnum>(ExternalGlobalEnums::EXTERNAL_TEST_ENUM_VALUE4));
    ASSERT_TRUE(test.externalEnum() == QVariant::fromValue<ExternalGlobalEnums::ExternalTestEnum>(ExternalGlobalEnums::EXTERNAL_TEST_ENUM_VALUE4));
}

TEST_F(SimpleTest, SimpleEnumsTest)
{
    EXPECT_GT(qtprotobufnamespace::tests::TestEnumGadget::staticMetaObject.enumeratorCount(), 0);
    QMetaEnum testEnum;
    for (int i = 0; i < qtprotobufnamespace::tests::TestEnumGadget::staticMetaObject.enumeratorCount(); i++) {
        QMetaEnum tmp = qtprotobufnamespace::tests::TestEnumGadget::staticMetaObject.enumerator(i);
        if (QString(tmp.name()) == QString("TestEnum")) {
            testEnum = tmp;
            break;
        }
    }
    ASSERT_TRUE(testEnum.isValid());
    ASSERT_STREQ(testEnum.key(0), "TEST_ENUM_VALUE0");
    ASSERT_STREQ(testEnum.key(1), "TEST_ENUM_VALUE1");
    ASSERT_STREQ(testEnum.key(2), "TEST_ENUM_VALUE2");
    ASSERT_STREQ(testEnum.key(3), "TEST_ENUM_VALUE3");
    ASSERT_STREQ(testEnum.key(4), "TEST_ENUM_VALUE4");

    ASSERT_EQ(testEnum.value(0), 0);
    ASSERT_EQ(testEnum.value(1), 1);
    ASSERT_EQ(testEnum.value(2), 2);
    ASSERT_EQ(testEnum.value(3), 4);
    ASSERT_EQ(testEnum.value(4), 3);

    for (int i = 0; i < TestEnumSecondInFileGadget::staticMetaObject.enumeratorCount(); i++) {
        QMetaEnum tmp = TestEnumSecondInFileGadget::staticMetaObject.enumerator(i);
        if (QString(tmp.name()) == QString("TestEnumSecondInFile")) {
            testEnum = tmp;
            break;
        }
    }

    ASSERT_TRUE(testEnum.isValid());
    ASSERT_STREQ(testEnum.key(0), "TEST_ENUM_SIF_VALUE0");
    ASSERT_STREQ(testEnum.key(1), "TEST_ENUM_SIF_VALUE1");
    ASSERT_STREQ(testEnum.key(2), "TEST_ENUM_SIF_VALUE2");

    ASSERT_EQ(testEnum.value(0), 0);
    ASSERT_EQ(testEnum.value(1), 1);
    ASSERT_EQ(testEnum.value(2), 2);
}

TEST_F(SimpleTest, SimpleFileEnumsTest)
{
    const char *propertyName = "globalEnumListData";
    assertMessagePropertyRegistered<SimpleFileEnumMessage, qtprotobufnamespace::tests::TestEnumGadget::TestEnumRepeated>(2, "qtprotobufnamespace::tests::TestEnumGadget::TestEnumRepeated", propertyName);

    qtprotobufnamespace::tests::TestEnumGadget::TestEnumRepeated value{qtprotobufnamespace::tests::TestEnumGadget::TEST_ENUM_VALUE1,
                                     qtprotobufnamespace::tests::TestEnumGadget::TEST_ENUM_VALUE3,
                                     qtprotobufnamespace::tests::TestEnumGadget::TEST_ENUM_VALUE4,
                                     qtprotobufnamespace::tests::TestEnumGadget::TEST_ENUM_VALUE2,
                                     qtprotobufnamespace::tests::TestEnumGadget::TEST_ENUM_VALUE1};
    SimpleFileEnumMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<qtprotobufnamespace::tests::TestEnumGadget::TestEnumRepeated>(value)));
    ASSERT_TRUE(test.property(propertyName).value<qtprotobufnamespace::tests::TestEnumGadget::TestEnumRepeated>() == value);
    ASSERT_TRUE(test.globalEnumList() == value);
}

TEST_F(SimpleTest, ComplexMessageTest)
{
    const char *propertyName = "testComplexField";
    assertMessagePropertyRegistered<qtprotobufnamespace::tests::ComplexMessage, qtprotobufnamespace::tests::SimpleStringMessage*>(
                2, "qtprotobufnamespace::tests::SimpleStringMessage*", propertyName);

    ComplexMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<qtprotobufnamespace::tests::SimpleStringMessage*>(new qtprotobufnamespace::tests::SimpleStringMessage{"test qwerty"})));
    ASSERT_TRUE(*(test.property(propertyName).value<qtprotobufnamespace::tests::SimpleStringMessage*>()) == qtprotobufnamespace::tests::SimpleStringMessage{"test qwerty"});
    ASSERT_TRUE(test.testComplexField() == qtprotobufnamespace::tests::SimpleStringMessage{"test qwerty"});
}

TEST_F(SimpleTest, SimpleBytesMessageTest)
{
    const char *propertyName = "testFieldBytes";
    SimpleBytesMessage test;
    int propertyNumber = SimpleBytesMessage::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_EQ(SimpleBytesMessage::staticMetaObject.property(propertyNumber).type(), QMetaType::QByteArray);
    ASSERT_STREQ(SimpleBytesMessage::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<QByteArray>("\x01\x02\x03\x04\x05")));
    ASSERT_TRUE(test.property(propertyName).toByteArray() == QByteArray("\x01\x02\x03\x04\x05"));
    ASSERT_TRUE(test.testFieldBytes() == QByteArray("\x01\x02\x03\x04\x05"));
}

TEST_F(SimpleTest, SimpleExternalComplexMessageTest)
{
    const char *propertyName = "localListData";
    assertMessagePropertyRegistered<qtprotobufnamespace1::externaltests::SimpleExternalMessage, int32List>(
                1, "QtProtobuf::int32List", propertyName);

    qtprotobufnamespace1::externaltests::SimpleExternalMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<int32List>({1, 2, 3, 4, 5})));
    ASSERT_TRUE(test.property(propertyName).value<int32List>() == int32List({1, 2, 3, 4, 5}));
    ASSERT_TRUE(test.localList() == int32List({1, 2, 3, 4, 5}));
}

TEST_F(SimpleTest, RepeatedExternalComplexMessageTest)
{
    const char *propertyName = "testExternalComplexData";
    assertMessagePropertyRegistered<RepeatedExternalComplexMessage, qtprotobufnamespace1::externaltests::ExternalComplexMessageRepeated>(
                1, "qtprotobufnamespace1::externaltests::ExternalComplexMessageRepeated", propertyName);

    qtprotobufnamespace1::externaltests::SimpleExternalMessage complexMessage;
    complexMessage.setLocalList({1, 2, 3, 4, 5});

    QSharedPointer<qtprotobufnamespace1::externaltests::ExternalComplexMessage> externalMessage(new qtprotobufnamespace1::externaltests::ExternalComplexMessage);
    externalMessage->setTestFieldInt(complexMessage);

    qtprotobufnamespace1::externaltests::ExternalComplexMessageRepeated complexMessageList;
    complexMessageList << externalMessage;

    RepeatedExternalComplexMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue(complexMessageList)));
    ASSERT_TRUE(test.property(propertyName).value<qtprotobufnamespace1::externaltests::ExternalComplexMessageRepeated>() == complexMessageList);
    ASSERT_TRUE(test.testExternalComplex() == complexMessageList);
}

TEST_F(SimpleTest, RepeatedStringMessageTest)
{
    const char *propertyName = "testRepeatedStringData";
    assertMessagePropertyRegistered<RepeatedStringMessage, QStringList>(1, "QStringList", propertyName);

    RepeatedStringMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<QStringList>({"Text", "tryam"})));
    ASSERT_TRUE(test.property(propertyName).value<QStringList>() == QStringList({"Text", "tryam"}));
    ASSERT_TRUE(test.testRepeatedString() == QStringList({"Text", "tryam"}));
}

TEST_F(SimpleTest, RepeatedIntMessageTest)
{
    const char *propertyName = "testRepeatedIntData";
    assertMessagePropertyRegistered<RepeatedIntMessage, int32List>(1, "QtProtobuf::int32List", propertyName);

    RepeatedIntMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<int32List>({1, 2, 3, 4, 5})));
    ASSERT_TRUE(test.property(propertyName).value<int32List>() == int32List({1, 2, 3, 4, 5}));
    ASSERT_TRUE(test.testRepeatedInt() == int32List({1, 2, 3, 4, 5}));

    test.testRepeatedInt().append(66);
    ASSERT_TRUE(test.testRepeatedInt() == int32List({1, 2, 3, 4, 5, 66}));

    test.testRepeatedInt().pop_back();
    ASSERT_TRUE(test.testRepeatedInt() == int32List({1, 2, 3, 4, 5}));
}

TEST_F(SimpleTest, RepeatedDoubleMessageTest)
{
    const char *propertyName = "testRepeatedDoubleData";
    assertMessagePropertyRegistered<RepeatedDoubleMessage, DoubleList>(1, "QtProtobuf::DoubleList", propertyName);

    RepeatedDoubleMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<QtProtobuf::DoubleList>({1.0, 2.3, 3, 4.7, 5.9})));
    ASSERT_TRUE(test.property(propertyName).value<QtProtobuf::DoubleList>() == QtProtobuf::DoubleList({1.0, 2.3, 3, 4.7, 5.9}));
    ASSERT_TRUE(test.testRepeatedDouble() == QtProtobuf::DoubleList({1.0, 2.3, 3, 4.7, 5.9}));

    test.testRepeatedDouble().append(6.6);
    ASSERT_TRUE(test.testRepeatedDouble() == QtProtobuf::DoubleList({1.0, 2.3, 3, 4.7, 5.9, 6.6}));

    test.testRepeatedDouble().pop_back();
    ASSERT_TRUE(test.testRepeatedDouble() == QtProtobuf::DoubleList({1.0, 2.3, 3, 4.7, 5.9}));
}

TEST_F(SimpleTest, RepeatedFloatMessageTest)
{
    const char *propertyName = "testRepeatedFloatData";
    assertMessagePropertyRegistered<RepeatedFloatMessage, FloatList>(1, "QtProtobuf::FloatList", propertyName);

    RepeatedFloatMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<QtProtobuf::FloatList>({1.0f, 2.3f, 3, 4.7f, 5.9f})));
    ASSERT_TRUE(test.property(propertyName).value<QtProtobuf::FloatList>() == QtProtobuf::FloatList({1.0f, 2.3f, 3, 4.7f, 5.9f}));
    ASSERT_TRUE(test.testRepeatedFloat() == QtProtobuf::FloatList({1.0f, 2.3f, 3, 4.7f, 5.9f}));

    test.testRepeatedFloat().append(6.6f);
    ASSERT_TRUE(test.testRepeatedFloat() == QtProtobuf::FloatList({1.0f, 2.3f, 3, 4.7f, 5.9f, 6.6f}));

    test.testRepeatedFloat().pop_back();
    ASSERT_TRUE(test.testRepeatedFloat() == QtProtobuf::FloatList({1.0f, 2.3f, 3, 4.7f, 5.9f}));
}

TEST_F(SimpleTest, RepeatedBytesMessageTest)
{
    const char *propertyName = "testRepeatedBytesData";
    assertMessagePropertyRegistered<RepeatedBytesMessage, QByteArrayList>(1, "QByteArrayList", propertyName);

    QByteArrayList bList;
    bList << "\x01\x02\x03\x04\x05";
    bList << "\x01\x05\x03\x04\x03";

    RepeatedBytesMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<QByteArrayList>(bList)));
    ASSERT_TRUE(test.property(propertyName).value<QByteArrayList>() == bList);
    ASSERT_TRUE(test.testRepeatedBytes() == bList);

    bList << "\x01\x05\x03\x03";
    test.testRepeatedBytes() << "\x01\x05\x03\x03";
    ASSERT_TRUE(test.testRepeatedBytes() == bList);

    bList.pop_back();
    test.testRepeatedBytes().pop_back();
    ASSERT_TRUE(test.testRepeatedBytes() == bList);
}

TEST_F(SimpleTest, RepeatedSIntMessageTest)
{
    const char *propertyName = "testRepeatedIntData";
    assertMessagePropertyRegistered<RepeatedSIntMessage, sint32List>(1, "QtProtobuf::sint32List", propertyName);

    RepeatedSIntMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<sint32List>({1, 2, 3, 4, 5})));
    ASSERT_TRUE(test.property(propertyName).value<sint32List>() == sint32List({1, 2, 3, 4, 5}));
    ASSERT_TRUE(test.testRepeatedInt() == sint32List({1, 2, 3, 4, 5}));

    test.testRepeatedInt() << 6;
    ASSERT_TRUE(test.testRepeatedInt() == sint32List({1, 2, 3, 4, 5, 6}));

    test.testRepeatedInt().pop_back();
    ASSERT_TRUE(test.testRepeatedInt() == sint32List({1, 2, 3, 4, 5}));
}

TEST_F(SimpleTest, RepeatedUIntMessageTest)
{
    const char *propertyName = "testRepeatedIntData";
    assertMessagePropertyRegistered<RepeatedUIntMessage, uint32List>(1, "QtProtobuf::uint32List", propertyName);

    RepeatedUIntMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<uint32List>({1, 2, 3, 4, 5})));
    ASSERT_TRUE(test.property(propertyName).value<uint32List>() == uint32List({1, 2, 3, 4, 5}));
    ASSERT_TRUE(test.testRepeatedInt() == uint32List({1, 2, 3, 4, 5}));

    test.testRepeatedInt().append(6);
    ASSERT_TRUE(test.testRepeatedInt() == uint32List({1, 2, 3, 4, 5,6}));

    test.testRepeatedInt().pop_back();
    ASSERT_TRUE(test.testRepeatedInt() == uint32List({1, 2, 3, 4, 5}));
}

TEST_F(SimpleTest, RepeatedInt64MessageTest)
{
    const char *propertyName = "testRepeatedIntData";
    assertMessagePropertyRegistered<RepeatedInt64Message, int64List>(1, "QtProtobuf::int64List", propertyName);

    RepeatedInt64Message test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<int64List>({1, 2, 3, 4, 5})));
    ASSERT_TRUE(test.property(propertyName).value<int64List>() == int64List({1, 2, 3, 4, 5}));
    ASSERT_TRUE(test.testRepeatedInt() == int64List({1, 2, 3, 4, 5}));

    test.testRepeatedInt().append(69);
    ASSERT_TRUE(test.testRepeatedInt() == int64List({1, 2, 3, 4, 5, 69}));

    test.testRepeatedInt().pop_back();
    ASSERT_TRUE(test.testRepeatedInt() == int64List({1, 2, 3, 4, 5}));
}

TEST_F(SimpleTest, RepeatedSInt64MessageTest)
{
    const char *propertyName = "testRepeatedIntData";
    assertMessagePropertyRegistered<RepeatedSInt64Message, sint64List>(1, "QtProtobuf::sint64List", propertyName);

    RepeatedSInt64Message test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<sint64List>({1, 2, 3, 4, 5})));
    ASSERT_TRUE(test.property(propertyName).value<sint64List>() == sint64List({1, 2, 3, 4, 5}));
    ASSERT_TRUE(test.testRepeatedInt() == sint64List({1, 2, 3, 4, 5}));

    test.testRepeatedInt() << 96;
    ASSERT_TRUE(test.testRepeatedInt() == sint64List({1, 2, 3, 4, 5, 96}));

    test.testRepeatedInt().pop_back();
    ASSERT_TRUE(test.testRepeatedInt() == sint64List({1, 2, 3, 4, 5}));
}

TEST_F(SimpleTest, RepeatedUInt64MessageTest)
{
    const char *propertyName = "testRepeatedIntData";
    assertMessagePropertyRegistered<RepeatedUInt64Message, uint64List>(1, "QtProtobuf::uint64List", propertyName);

    RepeatedUInt64Message test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<uint64List>({1, 2, 3, 4, 5})));
    ASSERT_TRUE(test.property(propertyName).value<uint64List>() == uint64List({1, 2, 3, 4, 5}));
    ASSERT_TRUE(test.testRepeatedInt() == uint64List({1, 2, 3, 4, 5}));

    test.testRepeatedInt().append(96);
    ASSERT_TRUE(test.testRepeatedInt() == uint64List({1, 2, 3, 4, 5, 96}));

    test.testRepeatedInt().pop_back();
    ASSERT_TRUE(test.testRepeatedInt() == uint64List({1, 2, 3, 4, 5}));
}

TEST_F(SimpleTest, RepeatedFixedIntMessageTest)
{
    const char *propertyName = "testRepeatedIntData";
    assertMessagePropertyRegistered<RepeatedFixedIntMessage, fixed32List>(1, "QtProtobuf::fixed32List", propertyName);

    RepeatedFixedIntMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<fixed32List>({1, 2, 3, 4, 5})));
    ASSERT_TRUE(test.property(propertyName).value<fixed32List>() == fixed32List({1, 2, 3, 4, 5}));
    ASSERT_TRUE(test.testRepeatedInt() == fixed32List({1, 2, 3, 4, 5}));

    test.testRepeatedInt() << 0;
    ASSERT_TRUE(test.testRepeatedInt() == fixed32List({1, 2, 3, 4, 5, 0}));

    test.testRepeatedInt().pop_back();
    ASSERT_TRUE(test.testRepeatedInt() == fixed32List({1, 2, 3, 4, 5}));
}

TEST_F(SimpleTest, RepeatedFixedInt64MessageTest)
{
    const char *propertyName = "testRepeatedIntData";
    assertMessagePropertyRegistered<RepeatedFixedInt64Message, fixed64List>(1, "QtProtobuf::fixed64List", propertyName);

    RepeatedFixedInt64Message test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<fixed64List>({1, 2, 3, 4, 5})));
    ASSERT_TRUE(test.property(propertyName).value<fixed64List>() == fixed64List({1, 2, 3, 4, 5}));
    ASSERT_TRUE(test.testRepeatedInt() == fixed64List({1, 2, 3, 4, 5}));

    test.testRepeatedInt() << 0;
    ASSERT_TRUE(test.testRepeatedInt() == fixed64List({1, 2, 3, 4, 5, 0}));

    test.testRepeatedInt().pop_back();
    ASSERT_TRUE(test.testRepeatedInt() == fixed64List({1, 2, 3, 4, 5}));
}

TEST_F(SimpleTest, RepeatedSFixedIntMessageTest)
{
    const char *propertyName = "testRepeatedIntData";
    assertMessagePropertyRegistered<RepeatedSFixedIntMessage, sfixed32List>(1, "QtProtobuf::sfixed32List", propertyName);

    RepeatedSFixedIntMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<sfixed32List>({1, 2, 3, 4, 5})));
    ASSERT_TRUE(test.property(propertyName).value<sfixed32List>() == sfixed32List({1, 2, 3, 4, 5}));
    ASSERT_TRUE(test.testRepeatedInt() == sfixed32List({1, 2, 3, 4, 5}));

    test.testRepeatedInt() << 0;
    ASSERT_TRUE(test.testRepeatedInt() == sfixed32List({1, 2, 3, 4, 5, 0}));

    test.testRepeatedInt().pop_back();
    ASSERT_TRUE(test.testRepeatedInt() == sfixed32List({1, 2, 3, 4, 5}));
}

TEST_F(SimpleTest, RepeatedSFixedInt64MessageTest)
{
    const char *propertyName = "testRepeatedIntData";
    assertMessagePropertyRegistered<RepeatedSFixedInt64Message, QtProtobuf::sfixed64List>(1, "QtProtobuf::sfixed64List", propertyName);

    RepeatedSFixedInt64Message test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<sfixed64List>({1, 2, 3, 4, 5})));
    ASSERT_TRUE(test.property(propertyName).value<sfixed64List>() == sfixed64List({1, 2, 3, 4, 5}));
    ASSERT_TRUE(test.testRepeatedInt() == sfixed64List({1, 2, 3, 4, 5}));

    test.testRepeatedInt() << 0;
    ASSERT_TRUE(test.testRepeatedInt() == sfixed64List({1, 2, 3, 4, 5, 0}));

    test.testRepeatedInt().pop_back();
    ASSERT_TRUE(test.testRepeatedInt() == sfixed64List({1, 2, 3, 4, 5}));
}

TEST_F(SimpleTest, StepChildEnumMessageTest)
{
    const char *propertyName = "localStepChildEnum";
    assertMessagePropertyRegistered<StepChildEnumMessage, SimpleEnumMessage::LocalEnum>(1, "qtprotobufnamespace::tests::SimpleEnumMessage::LocalEnum", propertyName);

    StepChildEnumMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<SimpleEnumMessage::LocalEnum>(SimpleEnumMessage::LocalEnum::LOCAL_ENUM_VALUE2)));
    ASSERT_TRUE(test.property(propertyName).value<SimpleEnumMessage::LocalEnum>() == SimpleEnumMessage::LocalEnum::LOCAL_ENUM_VALUE2);
    ASSERT_TRUE(test.localStepChildEnum() == SimpleEnumMessage::LocalEnum::LOCAL_ENUM_VALUE2);
}

TEST_F(SimpleTest, StepChildEnumListMessageTest)
{
    const char *propertyName = "localStepChildListData";
    assertMessagePropertyRegistered<StepChildEnumMessage, SimpleEnumMessage::LocalEnumRepeated>(2, "qtprotobufnamespace::tests::SimpleEnumMessage::LocalEnumRepeated", propertyName);

    SimpleEnumMessage::LocalEnumRepeated value({SimpleEnumMessage::LOCAL_ENUM_VALUE2,
                                            SimpleEnumMessage::LOCAL_ENUM_VALUE2,
                                            SimpleEnumMessage::LOCAL_ENUM_VALUE1,
                                            SimpleEnumMessage::LOCAL_ENUM_VALUE3});
    StepChildEnumMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<qtprotobufnamespace::tests::SimpleEnumMessage::LocalEnumRepeated>(value)));
    ASSERT_TRUE(test.property(propertyName).value<SimpleEnumMessage::LocalEnumRepeated>() == value);
    ASSERT_TRUE(test.localStepChildList() == value);
}


TEST_F(SimpleTest, SimpleSInt32StringMapMessageTest)
{
    const char *propertyName = "mapField";
    assertMessagePropertyRegistered<SimpleSInt32StringMapMessage, SimpleSInt32StringMapMessage::MapFieldEntry>(1, "qtprotobufnamespace::tests::SimpleSInt32StringMapMessage::MapFieldEntry", propertyName);

    ASSERT_TRUE(QMetaType::isRegistered(qMetaTypeId<SimpleSInt32StringMapMessage::MapFieldEntry>()));

    SimpleSInt32StringMapMessage::MapFieldEntry testMap = {{10, {"Some 10"}}, {0, {"Some 0"}}, {44, {"Some 44"}}};
    SimpleSInt32StringMapMessage test;
    test.setMapField(testMap);
    ASSERT_TRUE(test.property(propertyName).value<SimpleSInt32StringMapMessage::MapFieldEntry>() == testMap);
    ASSERT_TRUE(test.mapField() == testMap);
    ASSERT_STREQ(test.mapField()[10].toStdString().c_str(), "Some 10");
    ASSERT_STREQ(test.mapField()[0].toStdString().c_str(), "Some 0");
    ASSERT_STREQ(test.mapField()[44].toStdString().c_str(), "Some 44");

    test.mapField()[66] = "Some 66";
    ASSERT_STREQ(test.mapField()[66].toStdString().c_str(), "Some 66");

    test.mapField()[66] = "Some default";
    ASSERT_STREQ(test.mapField()[66].toStdString().c_str(), "Some default");
}

TEST_F(SimpleTest, SimpleStringStringMapMessageTest)
{
    const char *propertyName = "mapField";
    assertMessagePropertyRegistered<SimpleStringStringMapMessage, SimpleStringStringMapMessage::MapFieldEntry>(13, "qtprotobufnamespace::tests::SimpleStringStringMapMessage::MapFieldEntry", propertyName);

    ASSERT_TRUE(QMetaType::isRegistered(qMetaTypeId<SimpleStringStringMapMessage::MapFieldEntry>()));

    SimpleStringStringMapMessage::MapFieldEntry testMap = {{"key 10", "Some 10"}, {"key 0", "Some 0"}, {"key 44", "Some 44"}};
    SimpleStringStringMapMessage test;
    test.setMapField(testMap);
    ASSERT_TRUE(test.property(propertyName).value<SimpleStringStringMapMessage::MapFieldEntry>() == testMap);
    ASSERT_TRUE(test.mapField() == testMap);
    ASSERT_STREQ(test.mapField()["key 10"].toStdString().c_str(), "Some 10");
    ASSERT_STREQ(test.mapField()["key 0"].toStdString().c_str(), "Some 0");
    ASSERT_STREQ(test.mapField()["key 44"].toStdString().c_str(), "Some 44");

    test.mapField()["key 66"] = "Some 66";
    ASSERT_STREQ(test.mapField()["key 66"].toStdString().c_str(), "Some 66");

    test.mapField()["key 66"] = "Some default";
    ASSERT_STREQ(test.mapField()["key 66"].toStdString().c_str(), "Some default");
}

TEST_F(SimpleTest, EmptyMessageTest)
{
    ASSERT_EQ(qtprotobufnamespace::tests::EmptyMessage::propertyOrdering.size(), 0);
    ASSERT_EQ(qtprotobufnamespace::tests::EmptyMessage::staticMetaObject.propertyCount(), 1);
}

TEST_F(SimpleTest, AssignmentOperatorTest)
{
    const char *propertyName = "testFieldInt";
    qtprotobufnamespace::tests::SimpleIntMessage test;
    qtprotobufnamespace::tests::SimpleIntMessage test2{35};

    QSignalSpy updateSpy(&test, &qtprotobufnamespace::tests::SimpleIntMessage::testFieldIntChanged);
    test.setProperty(propertyName, QVariant::fromValue<int32>(15));
    test.setTestFieldInt(25);
    test = test2;
    test = test;
    test = test2;
    ASSERT_EQ(test2.testFieldInt(), test.testFieldInt());
    ASSERT_EQ(3, updateSpy.count());
}

TEST_F(SimpleTest, MoveOperatorTest)
{
    const char *propertyName = "testFieldInt";
    qtprotobufnamespace::tests::SimpleIntMessage test;
    qtprotobufnamespace::tests::SimpleIntMessage test2{35};

    QSignalSpy updateSpy(&test, &qtprotobufnamespace::tests::SimpleIntMessage::testFieldIntChanged);
    QSignalSpy movedUpdateSpy(&test2, &qtprotobufnamespace::tests::SimpleIntMessage::testFieldIntChanged);

    qtprotobufnamespace::tests::SimpleIntMessage test3(std::move(test2));
    test2.setTestFieldInt(35);

    test.setProperty(propertyName, QVariant::fromValue<int32>(15));
    test.setTestFieldInt(25);
    test = std::move(test2);
    ASSERT_EQ(35, test.testFieldInt());
    ASSERT_EQ(0, test2.testFieldInt());
    ASSERT_EQ(3, updateSpy.count());
    ASSERT_EQ(3, movedUpdateSpy.count());
}

TEST_F(SimpleTest, MoveOperatorRepeatedTest)
{
    const char *propertyName = "testRepeatedIntData";
    RepeatedIntMessage test;
    RepeatedIntMessage test2{{55,44,11,33}};

    QSignalSpy updateSpy(&test, &RepeatedIntMessage::testRepeatedIntChanged);
    QSignalSpy movedUpdateSpy(&test2, &RepeatedIntMessage::testRepeatedIntChanged);

    RepeatedIntMessage test3(std::move(test2));
    test2.setTestRepeatedInt({55,44,11,35});

    test.setProperty(propertyName, QVariant::fromValue<QtProtobuf::int32List>({55}));
    test.setTestRepeatedInt({44});
    test = std::move(test2);
    ASSERT_EQ(QtProtobuf::int32List({55,44,11,35}), test.testRepeatedInt());
    ASSERT_TRUE(test2.testRepeatedInt().isEmpty());
    ASSERT_EQ(3, updateSpy.count());
    ASSERT_EQ(3, movedUpdateSpy.count());
}

TEST_F(SimpleTest, UnderscoresTest)
{
    //Sanity compilation checks
    Message_Uderscore_name msg1;
    MessageUderscorename msg2;
    MessageUnderscoreField msg3;
    PriorMessageUnderscoreField msg4;
    FollowingMessageUnderscoreField msg5;
    CombinedMessageUnderscoreField msg6;

    assertMessagePropertyRegistered<MessageUnderscoreField, sint32>(1, "QtProtobuf::sint32", "underScoreMessageField");
    assertMessagePropertyRegistered<PriorMessageUnderscoreField, sint32>(1, "QtProtobuf::sint32", "underScoreMessageField");
    assertMessagePropertyRegistered<PriorMessageUnderscoreField, sint32>(1, "QtProtobuf::sint32", "underScoreMessageField");
    assertMessagePropertyRegistered<FollowingMessageUnderscoreField , sint32>(1, "QtProtobuf::sint32", "underScoreMessageField");
    assertMessagePropertyRegistered<CombinedMessageUnderscoreField , sint32>(1, "QtProtobuf::sint32", "underScoreMessageField");
}

TEST_F(SimpleTest, SequenceTest)
{
    assertMessagePropertyRegistered<sequence::TestMessageSequence, sequence::TestMessageSequence2*>(1, "qtprotobufnamespace::tests::sequence::TestMessageSequence2*", "testField");
    assertMessagePropertyRegistered<sequence::TestMessageSequence2, bool>(1, "bool", "testField");
}

TEST_F(SimpleTest, CyclingTest)
{
    assertMessagePropertyRegistered<sequence::CyclingFirstDependency, sequence::CyclingSecondDependency*>(1, "qtprotobufnamespace::tests::sequence::CyclingSecondDependency*", "testField");
    assertMessagePropertyRegistered<sequence::CyclingSecondDependency, sequence::CyclingFirstDependency*>(1, "qtprotobufnamespace::tests::sequence::CyclingFirstDependency*", "testField");
    sequence::CyclingFirstDependency test;
    sequence::CyclingSecondDependency test2;
    test.setTestField(test2);
    test2.setTestField(test);
}

TEST_F(SimpleTest, UpperCaseTest)
{
    assertMessagePropertyRegistered<MessageUpperCase, sint32>(1, "QtProtobuf::sint32", "testField");
}

TEST_F(SimpleTest, ReservedTest)
{
    assertMessagePropertyRegistered<MessageReserved, sint32>(1, "QtProtobuf::sint32", "import_proto");
    assertMessagePropertyRegistered<MessageReserved, sint32>(2, "QtProtobuf::sint32", "property_proto");
    assertMessagePropertyRegistered<MessageReserved, sint32>(3, "QtProtobuf::sint32", "id_proto");
}

TEST_F(SimpleTest, ReservedUpperCaseTest)
{
    assertMessagePropertyRegistered<MessageUpperCaseReserved, sint32>(1, "QtProtobuf::sint32", "import_proto");
    assertMessagePropertyRegistered<MessageUpperCaseReserved, sint32>(2, "QtProtobuf::sint32", "property_proto");
    assertMessagePropertyRegistered<MessageUpperCaseReserved, sint32>(3, "QtProtobuf::sint32", "id_proto");
}

TEST_F(SimpleTest, ReservedEnumTest)
{
    ASSERT_GT(MessageEnumReserved::staticMetaObject.enumeratorCount(), 0);
    QMetaEnum simpleEnum;
    for (int i = 0; i < MessageEnumReserved::staticMetaObject.enumeratorCount(); i++) {
        QMetaEnum tmp = MessageEnumReserved::staticMetaObject.enumerator(i);
        if (QString(tmp.name()) == QString("ReservedEnum")) {
            simpleEnum = tmp;
            break;
        }
    }
    ASSERT_TRUE(simpleEnum.isValid());
    ASSERT_STREQ(simpleEnum.key(0), "Import");
    ASSERT_STREQ(simpleEnum.key(1), "Property");
    ASSERT_STREQ(simpleEnum.key(2), "Id");

    ASSERT_EQ(simpleEnum.value(0), 0);
    ASSERT_EQ(simpleEnum.value(1), 1);
    ASSERT_EQ(simpleEnum.value(2), 2);
}

TEST_F(SimpleTest, LowerCaseEnumTest)
{
    ASSERT_GT(MessageEnumReserved::staticMetaObject.enumeratorCount(), 0);
    QMetaEnum simpleEnum;
    for (int i = 0; i < MessageEnumReserved::staticMetaObject.enumeratorCount(); i++) {
        QMetaEnum tmp = MessageEnumReserved::staticMetaObject.enumerator(i);
        if (QString(tmp.name()) == QString("LowerCaseEnum")) {
            simpleEnum = tmp;
            break;
        }
    }
    ASSERT_TRUE(simpleEnum.isValid());
    ASSERT_STREQ(simpleEnum.key(0), "EnumValue0");
    ASSERT_STREQ(simpleEnum.key(1), "EnumValue1");
    ASSERT_STREQ(simpleEnum.key(2), "EnumValue2");
}

TEST_F(SimpleTest, MapRepeatedFieldSequenceTest)
{
    assertMessagePropertyRegistered<sequence::RepeatedFieldSequence, sequence::RepeatedFieldSequence2Repeated>(1, "qtprotobufnamespace::tests::sequence::RepeatedFieldSequence2Repeated", "testFieldData");
    assertMessagePropertyRegistered<sequence::MapFieldSequence, sequence::MapFieldSequence::TestFieldEntry>(1, "qtprotobufnamespace::tests::sequence::MapFieldSequence::TestFieldEntry", "testField");
}

TEST_F(SimpleTest, NoPackageEmptyMessageMessageTest)
{
    ASSERT_EQ(::EmptyMessage::propertyOrdering.size(), 0);
    ASSERT_EQ(::EmptyMessage::staticMetaObject.propertyCount(), 1);
}

TEST_F(SimpleTest, NoPackageSimpleIntMessageTest)
{
    const char *propertyName = "testFieldInt_p";
    assertMessagePropertyRegistered<::SimpleIntMessage, int32>(1, "QtProtobuf::int32", propertyName);

    ::SimpleIntMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<int32>(1)));
    ASSERT_EQ(test.property(propertyName).value<int32>(), 1);
    ASSERT_EQ(test.testFieldInt(), 1);

    EXPECT_EQ(::SimpleIntMessage::TestFieldIntProtoFieldNumber, 1);
}

TEST_F(SimpleTest, NoPackageEnumTest)
{
    EXPECT_GT(::TestEnumGadget::staticMetaObject.enumeratorCount(), 0);
    QMetaEnum testEnum;
    for (int i = 0; i < ::TestEnumGadget::staticMetaObject.enumeratorCount(); i++) {
        QMetaEnum tmp = ::TestEnumGadget::staticMetaObject.enumerator(i);
        if (QString(tmp.name()) == QString("TestEnum")) {
            testEnum = tmp;
            break;
        }
    }
    ASSERT_EQ(testEnum.keyCount(), 4);
    EXPECT_TRUE(testEnum.isValid());
    EXPECT_STREQ(testEnum.key(0), "LOCAL_ENUM_VALUE0");
    EXPECT_STREQ(testEnum.key(1), "LOCAL_ENUM_VALUE1");
    EXPECT_STREQ(testEnum.key(2), "LOCAL_ENUM_VALUE2");
    EXPECT_STREQ(testEnum.key(3), "LOCAL_ENUM_VALUE3");

    EXPECT_EQ(testEnum.value(0), 0);
    EXPECT_EQ(testEnum.value(1), 1);
    EXPECT_EQ(testEnum.value(2), 2);
    EXPECT_EQ(testEnum.value(3), 5);
}

TEST_F(SimpleTest, NoPackageExternalTest)
{
    const char *propertyName = "testField";
    assertMessagePropertyRegistered<NoPackageExternalMessage, SimpleIntMessageExt*>(1, "SimpleIntMessageExt*", propertyName);

    NoPackageExternalMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<SimpleIntMessageExt*>(new SimpleIntMessageExt{42})));
    ASSERT_EQ(test.property(propertyName).value<SimpleIntMessageExt*>()->testFieldInt(), 42);
    ASSERT_EQ(test.testField().testFieldInt(), 42);
}


TEST_F(SimpleTest, NoPackageMessageTest)
{
    const char *propertyName = "testField";
    assertMessagePropertyRegistered<NoPackageMessage, SimpleIntMessageExt*>(1, "SimpleIntMessageExt*", propertyName);

    NoPackageMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<SimpleIntMessageExt*>(new SimpleIntMessageExt{42})));
    ASSERT_EQ(test.property(propertyName).value<SimpleIntMessageExt*>()->testFieldInt(), 42);
    ASSERT_EQ(test.testField().testFieldInt(), 42);
}
} // tests
} // qtprotobuf
