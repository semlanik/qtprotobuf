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

#include "simpletest.pb.h"

//#include "stepchildenummessage.h"
//#include "simpleboolmessage.h"
//#include "simpleintmessage.h"
//#include "simplesintmessage.h"
//#include "simpleuintmessage.h"
//#include "simpleint64message.h"
//#include "simplesint64message.h"
//#include "simpleuint64message.h"
//#include "simplefixedint32message.h"
//#include "simplefixedint64message.h"
//#include "simplesfixedint32message.h"
//#include "simplesfixedint64message.h"
//#include "simplestringmessage.h"
//#include "simplefloatmessage.h"
//#include "simpledoublemessage.h"
//#include "simpleenummessage.h"
//#include "simpleenumlistmessage.h"
//#include "simplefileenummessage.h"
//#include "simpleexternalenummessage.h"
//#include "externalcomplexmessage.h"
//#include "complexmessage.h"
//#include "simplebytesmessage.h"
//#include "repeatedintmessage.h"
//#include "repeatedbytesmessage.h"
//#include "repeateddoublemessage.h"
//#include "repeatedcomplexmessage.h"
//#include "repeatedfloatmessage.h"
//#include "repeatedsintmessage.h"
//#include "repeatedstringmessage.h"
//#include "repeateduintmessage.h"
//#include "repeatedint64message.h"
//#include "repeatedsint64message.h"
//#include "repeateduint64message.h"
//#include "repeatedfixedintmessage.h"
//#include "repeatedsfixedintmessage.h"
//#include "repeatedfixedint64message.h"
//#include "repeatedsfixedint64message.h"
//#include "repeatedexternalcomplexmessage.h"
//#include "simplesint32stringmapmessage.h"
//#include "simplestringstringmapmessage.h"
//#include "emptymessage.h"
//#include "message_uderscore_name.h"
//#include "messageuderscorename.h"
//#include "messageunderscorefield.h"
//#include "priormessageunderscorefield.h"
//#include "followingmessageunderscorefield.h"
//#include "combinedmessageunderscorefield.h"

//#include "globalenums.h"
#include <QVariantList>
#include <QMetaProperty>
#include <QSignalSpy>

#include <gtest/gtest.h>

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

    template<typename MessageType, typename PropertyType>
    static void assertMessagePropertyRegistered(int fieldIndex, const char *propertyTypeName, const char *propertyName)
    {
        // TODO: there should be(?) a mapping avaialble: PropertyType -> propertyTypeName

        const int propertyNumber = MessageType::propertyOrdering.at(fieldIndex);
        ASSERT_STREQ(MessageType::staticMetaObject.property(propertyNumber).typeName(), propertyTypeName);
        ASSERT_EQ(MessageType::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<PropertyType>());
        ASSERT_STREQ(MessageType::staticMetaObject.property(propertyNumber).name(), propertyName);
    }

    static void SetUpTestCase();
};

void SimpleTest::SetUpTestCase()
{
    QtProtobuf::registerProtoTypes();
    qRegisterProtobufType<SimpleIntMessage>();
    qRegisterProtobufType<SimpleUIntMessage>();
    qRegisterProtobufType<SimpleSIntMessage>();
    qRegisterProtobufType<SimpleInt64Message>();
    qRegisterProtobufType<SimpleUInt64Message>();
    qRegisterProtobufType<SimpleSInt64Message>();
    qRegisterProtobufType<SimpleFixedInt32Message>();
    qRegisterProtobufType<SimpleFixedInt64Message>();
    qRegisterProtobufType<SimpleSFixedInt32Message>();
    qRegisterProtobufType<SimpleSFixedInt64Message>();
    qRegisterProtobufType<SimpleFloatMessage>();
    qRegisterProtobufType<SimpleDoubleMessage>();
    qRegisterProtobufType<SimpleStringMessage>();
    qRegisterProtobufType<ComplexMessage>();
    qRegisterProtobufType<RepeatedIntMessage>();
    qRegisterProtobufType<RepeatedSIntMessage>();
    qRegisterProtobufType<RepeatedUIntMessage>();
    qRegisterProtobufType<RepeatedInt64Message>();
    qRegisterProtobufType<RepeatedSInt64Message>();
    qRegisterProtobufType<RepeatedUInt64Message>();
    qRegisterProtobufType<RepeatedFixedIntMessage>();
    qRegisterProtobufType<RepeatedSFixedIntMessage>();
    qRegisterProtobufType<RepeatedFixedInt64Message>();
    qRegisterProtobufType<RepeatedStringMessage>();
    qRegisterProtobufType<RepeatedDoubleMessage>();
    qRegisterProtobufType<RepeatedBytesMessage>();
    qRegisterProtobufType<RepeatedFloatMessage>();
    qRegisterProtobufType<RepeatedComplexMessage>();
//    qRegisterProtobufType<SimpleFixed32StringMapMessage>();
//    qRegisterProtobufType<SimpleSFixed32StringMapMessage>();
//    qRegisterProtobufType<SimpleInt32StringMapMessage>();
    qRegisterProtobufType<SimpleSInt32StringMapMessage>();
//    qRegisterProtobufType<SimpleUInt32StringMapMessage>();
//    qRegisterProtobufType<SimpleFixed64StringMapMessage>();
//    qRegisterProtobufType<SimpleSFixed64StringMapMessage>();
//    qRegisterProtobufType<SimpleInt64StringMapMessage>();
//    qRegisterProtobufType<SimpleSInt64StringMapMessage>();
//    qRegisterProtobufType<SimpleUInt64StringMapMessage>();
    qRegisterProtobufType<SimpleStringStringMapMessage>();
//    qRegisterProtobufType<FieldIndexTest1Message>();
//    qRegisterProtobufType<FieldIndexTest2Message>();
//    qRegisterProtobufType<FieldIndexTest3Message>();
//    qRegisterProtobufType<FieldIndexTest4Message>();
    qRegisterProtobufType<SimpleEnumListMessage>();
    qRegisterProtobufType<SimpleBytesMessage>();

    qRegisterProtobufType<EmptyMessage>();
    qRegisterProtobufType<Message_Uderscore_name>();
    qRegisterProtobufType<MessageUderscorename>();
    qRegisterProtobufType<MessageUnderscoreField>();
    qRegisterProtobufType<PriorMessageUnderscoreField>();
    qRegisterProtobufType<FollowingMessageUnderscoreField>();
    qRegisterProtobufType<CombinedMessageUnderscoreField>();
}

TEST_F(SimpleTest, SimpleBoolMessageTest)
{
    const char *propertyName = "testFieldBool";
    assertMessagePropertyRegistered<SimpleBoolMessage, bool>(1, "bool", propertyName);

    SimpleBoolMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue(true)));
    ASSERT_EQ(test.property(propertyName).value<bool>(), true);
    ASSERT_EQ(test.testFieldBool(), true);
}

TEST_F(SimpleTest, SimpleIntMessageTest)
{
    const char *propertyName = "testFieldInt";
    assertMessagePropertyRegistered<SimpleIntMessage, int32>(1, "QtProtobuf::int32", propertyName);

    SimpleIntMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<int32>(1)));
    ASSERT_EQ(test.property(propertyName).value<int32>(), 1);
    ASSERT_EQ(test.testFieldInt(), 1);
}

TEST_F(SimpleTest, SimpleSIntMessageTest)
{
    const char *propertyName = "testFieldInt";
    assertMessagePropertyRegistered<SimpleSIntMessage, sint32>(1, "QtProtobuf::sint32", propertyName);

    SimpleSIntMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<sint32>(1)));
    ASSERT_EQ(test.property(propertyName).value<sint32>(), 1);
    ASSERT_EQ(test.testFieldInt(), 1);
}

TEST_F(SimpleTest, SimpleUIntMessageTest)
{
    const char *propertyName = "testFieldInt";
    assertMessagePropertyRegistered<SimpleUIntMessage, uint32>(1, "QtProtobuf::uint32", propertyName);

    SimpleUIntMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<uint32>(1)));
    ASSERT_EQ(test.property(propertyName).value<uint32>(), 1);
    ASSERT_EQ(test.testFieldInt(), 1);
}

TEST_F(SimpleTest, SimpleInt64MessageTest)
{
    const char *propertyName = "testFieldInt";
    assertMessagePropertyRegistered<SimpleInt64Message, int64>(1, "QtProtobuf::int64", propertyName);

    SimpleInt64Message test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<int64>(1)));
    ASSERT_EQ(test.property(propertyName).value<int64>(), 1);
    ASSERT_EQ(test.testFieldInt(), 1);
}

TEST_F(SimpleTest, SimpleSInt64MessageTest)
{
    const char *propertyName = "testFieldInt";
    assertMessagePropertyRegistered<SimpleSInt64Message, sint64>(1, "QtProtobuf::sint64", propertyName);

    SimpleSInt64Message test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<sint64>(1)));
    ASSERT_EQ(test.property(propertyName).value<sint64>(), 1);
    ASSERT_EQ(test.testFieldInt(), 1);
}

TEST_F(SimpleTest, SimpleUInt64MessageTest)
{
    const char *propertyName = "testFieldInt";
    assertMessagePropertyRegistered<SimpleUInt64Message, uint64>(1, "QtProtobuf::uint64", propertyName);

    SimpleUInt64Message test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<uint64>(1)));
    ASSERT_EQ(test.property(propertyName).value<uint64>(), 1);
    ASSERT_EQ(test.testFieldInt(), 1);
}

TEST_F(SimpleTest, SimpleFixedInt32MessageTest)
{
    const char *propertyName = "testFieldFixedInt32";
    assertMessagePropertyRegistered<SimpleFixedInt32Message, fixed32>(1, "QtProtobuf::fixed32", propertyName);

    SimpleFixedInt32Message test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<fixed32>(1)));
    ASSERT_EQ(test.property(propertyName).value<fixed32>(), 1);
    ASSERT_EQ(test.testFieldFixedInt32(), 1);
}

TEST_F(SimpleTest, SimpleFixedInt64MessageTest)
{
    const char *propertyName = "testFieldFixedInt64";
    assertMessagePropertyRegistered<SimpleFixedInt64Message, fixed64>(1, "QtProtobuf::fixed64", propertyName);

    SimpleFixedInt64Message test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<fixed64>(1)));
    ASSERT_EQ(test.property(propertyName).value<fixed64>(), 1);
    ASSERT_EQ(test.testFieldFixedInt64(), 1);
}

TEST_F(SimpleTest, SimpleSFixedInt32MessageTest)
{
    const char *propertyName = "testFieldFixedInt32";
    assertMessagePropertyRegistered<SimpleSFixedInt32Message, sfixed32>(1, "QtProtobuf::sfixed32", propertyName);

    SimpleSFixedInt32Message test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<sfixed32>(1)));
    ASSERT_EQ(test.property(propertyName).value<sfixed32>(), 1);
    ASSERT_EQ(test.testFieldFixedInt32(), 1);
}

TEST_F(SimpleTest, SimpleSFixedInt64MessageTest)
{
    const char *propertyName = "testFieldFixedInt64";
    assertMessagePropertyRegistered<SimpleSFixedInt64Message, sfixed64>(1, "QtProtobuf::sfixed64", propertyName);

    SimpleSFixedInt64Message test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<sfixed64>(1)));
    ASSERT_EQ(test.property(propertyName).value<sfixed64>(), 1);
    ASSERT_EQ(test.testFieldFixedInt64(), 1);
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

    const char *propertyName = "localEnumList";
    assertMessagePropertyRegistered<SimpleEnumListMessage, SimpleEnumListMessage::LocalEnumRepeated>(1,  "SimpleEnumListMessage::LocalEnumRepeated", propertyName);

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
    using ExternalGlobalEnums = qtprotobufnamespace1::externaltests::GlobalEnums;

    const char *propertyName = "externalEnum";
    assertMessagePropertyRegistered<SimpleExternalEnumMessage, ExternalGlobalEnums::ExternalTestEnum>(1, "qtprotobufnamespace1::externaltests::GlobalEnums::ExternalTestEnum", propertyName);

    SimpleExternalEnumMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<ExternalGlobalEnums::ExternalTestEnum>(ExternalGlobalEnums::EXTERNAL_TEST_ENUM_VALUE4)));
    ASSERT_TRUE(test.property(propertyName).value<ExternalGlobalEnums::ExternalTestEnum>() == QVariant::fromValue<ExternalGlobalEnums::ExternalTestEnum>(ExternalGlobalEnums::EXTERNAL_TEST_ENUM_VALUE4));
    ASSERT_TRUE(test.externalEnum() == QVariant::fromValue<ExternalGlobalEnums::ExternalTestEnum>(ExternalGlobalEnums::EXTERNAL_TEST_ENUM_VALUE4));
}

TEST_F(SimpleTest, SimpleEnumsTest)
{
    EXPECT_GT(GlobalEnums::staticMetaObject.enumeratorCount(), 0);
    QMetaEnum testEnum;
    for (int i = 0; i < GlobalEnums::staticMetaObject.enumeratorCount(); i++) {
        QMetaEnum tmp = GlobalEnums::staticMetaObject.enumerator(i);
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

    for (int i = 0; i < GlobalEnums::staticMetaObject.enumeratorCount(); i++) {
        QMetaEnum tmp = GlobalEnums::staticMetaObject.enumerator(i);
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
    const char *propertyName = "globalEnumList";
    assertMessagePropertyRegistered<SimpleFileEnumMessage, GlobalEnums::TestEnumRepeated>(2, "qtprotobufnamespace::tests::GlobalEnums::TestEnumRepeated", propertyName);

    GlobalEnums::TestEnumRepeated value({GlobalEnums::TEST_ENUM_VALUE1,
                                     GlobalEnums::TEST_ENUM_VALUE3,
                                     GlobalEnums::TEST_ENUM_VALUE4,
                                     GlobalEnums::TEST_ENUM_VALUE2,
                                     GlobalEnums::TEST_ENUM_VALUE1});
    SimpleFileEnumMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<GlobalEnums::TestEnumRepeated>(value)));
    ASSERT_TRUE(test.property(propertyName).value<GlobalEnums::TestEnumRepeated>() == value);
    ASSERT_TRUE(test.globalEnumList() == value);
}

TEST_F(SimpleTest, ComplexMessageTest)
{
    ComplexMessage msg;
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
    const char *propertyName = "localList";
    assertMessagePropertyRegistered<qtprotobufnamespace1::externaltests::SimpleExternalMessage, int32List>(
                1, "QtProtobuf::int32List", propertyName);

    qtprotobufnamespace1::externaltests::SimpleExternalMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<int32List>({1, 2, 3, 4, 5})));
    ASSERT_TRUE(test.property(propertyName).value<int32List>() == int32List({1, 2, 3, 4, 5}));
    ASSERT_TRUE(test.localList() == int32List({1, 2, 3, 4, 5}));
}

TEST_F(SimpleTest, RepeatedExternalComplexMessageTest)
{
    const char *propertyName = "testExternalComplex";
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
    const char *propertyName = "testRepeatedString";
    assertMessagePropertyRegistered<RepeatedStringMessage, QStringList>(1, "QStringList", propertyName);

    RepeatedStringMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<QStringList>({"Text", "tryam"})));
    ASSERT_TRUE(test.property(propertyName).value<QStringList>() == QStringList({"Text", "tryam"}));
    ASSERT_TRUE(test.testRepeatedString() == QStringList({"Text", "tryam"}));
}

TEST_F(SimpleTest, RepeatedIntMessageTest)
{
    const char *propertyName = "testRepeatedInt";
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
    const char *propertyName = "testRepeatedDouble";
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
    const char *propertyName = "testRepeatedFloat";
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
    const char *propertyName = "testRepeatedBytes";
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
    const char *propertyName = "testRepeatedInt";
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
    const char *propertyName = "testRepeatedInt";
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
    const char *propertyName = "testRepeatedInt";
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
    const char *propertyName = "testRepeatedInt";
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
    const char *propertyName = "testRepeatedInt";
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
    const char *propertyName = "testRepeatedInt";
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
    const char *propertyName = "testRepeatedInt";
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
    const char *propertyName = "testRepeatedInt";
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
    const char *propertyName = "testRepeatedInt";
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
    const char *propertyName = "localStepChildList";
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
    assertMessagePropertyRegistered<SimpleSInt32StringMapMessage, SimpleSInt32StringMapMessage::MapFieldEntry>(1, "SimpleSInt32StringMapMessage::MapFieldEntry", propertyName);

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
    assertMessagePropertyRegistered<SimpleStringStringMapMessage, SimpleStringStringMapMessage::MapFieldEntry>(13, "SimpleStringStringMapMessage::MapFieldEntry", propertyName);

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
    ASSERT_EQ(EmptyMessage::propertyOrdering.size(), 0);
    ASSERT_EQ(EmptyMessage::staticMetaObject.propertyCount(), 1);
}

TEST_F(SimpleTest, NullPointerMessageTest)
{
    ComplexMessage msg(0, {QString("not null")});
    msg.setTestComplexField_p(nullptr);
    ASSERT_TRUE(msg.testComplexField().testFieldString().isEmpty());
}

TEST_F(SimpleTest, AssignmentOperatorTest)
{
    const char *propertyName = "testFieldInt";
    SimpleIntMessage test;
    SimpleIntMessage test2{35};

    QSignalSpy updateSpy(&test, &SimpleIntMessage::testFieldIntChanged);
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
    SimpleIntMessage test;
    SimpleIntMessage test2{35};

    QSignalSpy updateSpy(&test, &SimpleIntMessage::testFieldIntChanged);
    QSignalSpy movedUpdateSpy(&test2, &SimpleIntMessage::testFieldIntChanged);

    SimpleIntMessage test3(std::move(test2));
    test2.setTestFieldInt(35);

    test.setProperty(propertyName, QVariant::fromValue<int32>(15));
    test.setTestFieldInt(25);
    test = std::move(test2);
    ASSERT_EQ(35, test.testFieldInt());
    ASSERT_EQ(0, test2.testFieldInt());
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

    assertMessagePropertyRegistered<MessageUnderscoreField, sint32>(1, "QtProtobuf::sint32", "underScore_Message_field");
    assertMessagePropertyRegistered<PriorMessageUnderscoreField, sint32>(1, "QtProtobuf::sint32", "_underScoreMessageField");
    assertMessagePropertyRegistered<PriorMessageUnderscoreField, sint32>(1, "QtProtobuf::sint32", "_underScoreMessageField");
    assertMessagePropertyRegistered<FollowingMessageUnderscoreField , sint32>(1, "QtProtobuf::sint32", "underScoreMessageField_");
    assertMessagePropertyRegistered<CombinedMessageUnderscoreField , sint32>(1, "QtProtobuf::sint32", "_underScoreMessage_Field_");
}

} // tests
} // qtprotobuf
