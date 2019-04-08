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

#include "simpletest.h"

#include "stepchildenummessage.h"
#include "simpleboolmessage.h"
#include "simpleintmessage.h"
#include "simplesintmessage.h"
#include "simpleuintmessage.h"
#include "simpleint64message.h"
#include "simplesint64message.h"
#include "simpleuint64message.h"
#include "simplefixedint32message.h"
#include "simplefixedint64message.h"
#include "simplesfixedint32message.h"
#include "simplesfixedint64message.h"
#include "simplestringmessage.h"
#include "simplefloatmessage.h"
#include "simpledoublemessage.h"
#include "simpleenummessage.h"
#include "simpleenumlistmessage.h"
#include "simplefileenummessage.h"
#include "simpleexternalenummessage.h"
#include "externalcomplexmessage.h"
#include "complexmessage.h"
#include "simplebytesmessage.h"
#include "repeatedintmessage.h"
#include "repeatedbytesmessage.h"
#include "repeateddoublemessage.h"
#include "repeatedcomplexmessage.h"
#include "repeatedfloatmessage.h"
#include "repeatedsintmessage.h"
#include "repeatedstringmessage.h"
#include "repeateduintmessage.h"
#include "repeatedint64message.h"
#include "repeatedsint64message.h"
#include "repeateduint64message.h"
#include "repeatedfixedintmessage.h"
#include "repeatedsfixedintmessage.h"
#include "repeatedfixedint64message.h"
#include "repeatedsfixedint64message.h"
#include "repeatedexternalcomplexmessage.h"
#include "simplestringmapmessage.h"
#include "simplesint32mapmessage.h"

#include "globalenums.h"
#include "qtprotobuf.h"
#include <QVariantList>
#include <QMetaProperty>

using namespace qtprotobufnamespace::tests;
using namespace qtprotobuf::tests;

using namespace qtprotobuf;

SimpleTest::SimpleTest()
{
    QtProtobuf::init();
}

TEST_F(SimpleTest, SimpleBoolMessageTest)
{
    const char* propertyName = "testFieldBool";
    SimpleBoolMessage test;
    int propertyNumber = SimpleBoolMessage::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_STREQ(SimpleBoolMessage::staticMetaObject.property(propertyNumber).typeName(), "bool");
    ASSERT_EQ(SimpleBoolMessage::staticMetaObject.property(propertyNumber).type(), qMetaTypeId<bool>());
    ASSERT_STREQ(SimpleBoolMessage::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue(true)));
    ASSERT_EQ(test.property(propertyName).toBool(), true);
    ASSERT_EQ(test.testFieldBool(), true);
}

TEST_F(SimpleTest, SimpleIntMessageTest)
{
    const char* propertyName = "testFieldInt";
    SimpleIntMessage test;
    int propertyNumber = SimpleIntMessage::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_STREQ(SimpleIntMessage::staticMetaObject.property(propertyNumber).typeName(), "qtprotobuf::int32");
    ASSERT_EQ(SimpleIntMessage::staticMetaObject.property(propertyNumber).type(), qMetaTypeId<int32>());
    ASSERT_STREQ(SimpleIntMessage::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue(1)));
    ASSERT_EQ(test.property(propertyName).toInt(), 1);
    ASSERT_EQ(test.testFieldInt(), 1);
}

TEST_F(SimpleTest, SimpleSIntMessageTest)
{
    const char* propertyName = "testFieldInt";
    SimpleSIntMessage test;
    int propertyNumber = SimpleSIntMessage::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_STREQ(SimpleSIntMessage::staticMetaObject.property(propertyNumber).typeName(), "qtprotobuf::sint32");
    ASSERT_EQ(SimpleSIntMessage::staticMetaObject.property(propertyNumber).type(), qMetaTypeId<sint32>());
    ASSERT_STREQ(SimpleSIntMessage::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue(1)));
    ASSERT_EQ(test.property(propertyName).toInt(), 1);
    ASSERT_EQ(test.testFieldInt(), 1);
}

TEST_F(SimpleTest, SimpleUIntMessageTest)
{
    const char* propertyName = "testFieldInt";
    SimpleUIntMessage test;
    int propertyNumber = SimpleUIntMessage::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_STREQ(SimpleUIntMessage::staticMetaObject.property(propertyNumber).typeName(), "qtprotobuf::uint32");
    ASSERT_EQ(SimpleUIntMessage::staticMetaObject.property(propertyNumber).type(), qMetaTypeId<uint32>());
    ASSERT_STREQ(SimpleUIntMessage::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue(1)));
    ASSERT_EQ(test.property(propertyName).toInt(), 1);
    ASSERT_EQ(test.testFieldInt(), 1);
}

TEST_F(SimpleTest, SimpleInt64MessageTest)
{
    const char* propertyName = "testFieldInt";
    SimpleInt64Message test;
    int propertyNumber = SimpleInt64Message::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_STREQ(SimpleInt64Message::staticMetaObject.property(propertyNumber).typeName(), "qtprotobuf::int64");
    ASSERT_EQ(SimpleInt64Message::staticMetaObject.property(propertyNumber).type(), qMetaTypeId<int64>());
    ASSERT_STREQ(SimpleInt64Message::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue(1)));
    ASSERT_EQ(test.property(propertyName).toInt(), 1);
    ASSERT_EQ(test.testFieldInt(), 1);
}

TEST_F(SimpleTest, SimpleSInt64MessageTest)
{
    const char* propertyName = "testFieldInt";
    SimpleSInt64Message test;
    int propertyNumber = SimpleSInt64Message::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_STREQ(SimpleSInt64Message::staticMetaObject.property(propertyNumber).typeName(), "qtprotobuf::sint64");
    ASSERT_EQ(SimpleSInt64Message::staticMetaObject.property(propertyNumber).type(), qMetaTypeId<sint64>());
    ASSERT_STREQ(SimpleSInt64Message::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue(1)));
    ASSERT_EQ(test.property(propertyName).toInt(), 1);
    ASSERT_EQ(test.testFieldInt(), 1);
}

TEST_F(SimpleTest, SimpleUInt64MessageTest)
{
    const char* propertyName = "testFieldInt";
    SimpleUInt64Message test;
    int propertyNumber = SimpleUInt64Message::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_STREQ(SimpleUInt64Message::staticMetaObject.property(propertyNumber).typeName(), "qtprotobuf::uint64");
    ASSERT_EQ(SimpleUInt64Message::staticMetaObject.property(propertyNumber).type(), qMetaTypeId<uint64>());
    ASSERT_STREQ(SimpleUInt64Message::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue(1)));
    ASSERT_EQ(test.property(propertyName).toInt(), 1);
    ASSERT_EQ(test.testFieldInt(), 1);
}

TEST_F(SimpleTest, SimpleFixedInt32MessageTest)
{
    const char* propertyName = "testFieldFixedInt32";
    SimpleFixedInt32Message test;
    int propertyNumber = SimpleFixedInt32Message::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_EQ(SimpleFixedInt32Message::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<fint32>());
    ASSERT_STREQ(SimpleFixedInt32Message::staticMetaObject.property(propertyNumber).typeName(), "qtprotobuf::fint32");
    ASSERT_STREQ(SimpleFixedInt32Message::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<fint32>(1)));
    ASSERT_EQ(test.property(propertyName).value<fint32>(), 1);
    ASSERT_EQ(test.testFieldFixedInt32(), 1);
}

TEST_F(SimpleTest, SimpleFixedInt64MessageTest)
{
    const char* propertyName = "testFieldFixedInt64";
    SimpleFixedInt64Message test;
    int propertyNumber = SimpleFixedInt64Message::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_EQ(SimpleFixedInt64Message::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<fint64>());
    ASSERT_STREQ(SimpleFixedInt64Message::staticMetaObject.property(propertyNumber).typeName(), "qtprotobuf::fint64");
    ASSERT_STREQ(SimpleFixedInt64Message::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<fint64>(1)));
    ASSERT_EQ(test.property(propertyName).value<fint64>(), 1);
    ASSERT_EQ(test.testFieldFixedInt64(), 1);
}

TEST_F(SimpleTest, SimpleSFixedInt32MessageTest)
{
    const char* propertyName = "testFieldFixedInt32";
    SimpleSFixedInt32Message test;
    int propertyNumber = SimpleSFixedInt32Message::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_EQ(SimpleSFixedInt32Message::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<sfint32>());
    ASSERT_STREQ(SimpleSFixedInt32Message::staticMetaObject.property(propertyNumber).typeName(), "qtprotobuf::sfint32");
    ASSERT_STREQ(SimpleSFixedInt32Message::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<sfint32>(1)));
    ASSERT_EQ(test.property(propertyName).value<sfint32>(), 1);
    ASSERT_EQ(test.testFieldFixedInt32(), 1);
}

TEST_F(SimpleTest, SimpleSFixedInt64MessageTest)
{
    const char* propertyName = "testFieldFixedInt64";
    SimpleSFixedInt64Message test;
    int propertyNumber = SimpleSFixedInt64Message::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_EQ(SimpleSFixedInt64Message::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<sfint64>());
    ASSERT_STREQ(SimpleSFixedInt64Message::staticMetaObject.property(propertyNumber).typeName(), "qtprotobuf::sfint64");
    ASSERT_STREQ(SimpleSFixedInt64Message::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<sfint64>(1)));
    ASSERT_EQ(test.property(propertyName).value<sfint64>(), 1);
    ASSERT_EQ(test.testFieldFixedInt64(), 1);
}

TEST_F(SimpleTest, SimpleStringMessageTest)
{
    const char* propertyName = "testFieldString";
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
    const char* propertyName = "testFieldFloat";
    SimpleFloatMessage test;
    int propertyNumber = SimpleFloatMessage::propertyOrdering.at(7); //See simpletest.proto
    ASSERT_EQ(SimpleFloatMessage::staticMetaObject.property(propertyNumber).type(), QMetaType::Float);
    ASSERT_STREQ(SimpleFloatMessage::staticMetaObject.property(propertyNumber).name(), "testFieldFloat");
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<float>(1.55f)));
    ASSERT_TRUE(qFuzzyCompare(test.property(propertyName).toFloat(), 1.55f));
    ASSERT_TRUE(qFuzzyCompare(test.testFieldFloat(), 1.55f));
}

TEST_F(SimpleTest, SimpleDoubleMessageTest)
{
    const char* propertyName = "testFieldDouble";
    SimpleDoubleMessage test;
    int propertyNumber = SimpleDoubleMessage::propertyOrdering.at(8); //See simpletest.proto
    ASSERT_EQ(SimpleDoubleMessage::staticMetaObject.property(propertyNumber).type(), QMetaType::Double);
    ASSERT_STREQ(SimpleDoubleMessage::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<double>(0.55)));
    ASSERT_FLOAT_EQ(test.property(propertyName).toDouble(), 0.55);
    ASSERT_FLOAT_EQ(test.testFieldDouble(), 0.55);
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
    const char* propertyName = "localEnumList";
    SimpleEnumListMessage test;
    int propertyNumber = SimpleEnumListMessage::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_STREQ(SimpleEnumListMessage::staticMetaObject.property(propertyNumber).typeName(), "SimpleEnumListMessage::LocalEnumList");
    ASSERT_EQ(SimpleEnumListMessage::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<qtprotobufnamespace::tests::SimpleEnumListMessage::LocalEnumList>());
    ASSERT_STREQ(SimpleEnumListMessage::staticMetaObject.property(propertyNumber).name(), propertyName);

    SimpleEnumListMessage::LocalEnumList value({SimpleEnumListMessage::LOCAL_ENUM_VALUE2,
                                                SimpleEnumListMessage::LOCAL_ENUM_VALUE2,
                                                SimpleEnumListMessage::LOCAL_ENUM_VALUE1,
                                                SimpleEnumListMessage::LOCAL_ENUM_VALUE3});

    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<SimpleEnumListMessage::LocalEnumList>(value)));
    ASSERT_TRUE(test.property(propertyName).value<SimpleEnumListMessage::LocalEnumList>() == value);
    ASSERT_TRUE(test.localEnumList() == value);
}

TEST_F(SimpleTest, SimpleExternalEnumMessageTest)
{
    const char* propertyName = "externalEnum";
    using ExternalGlobalEnums = qtprotobufnamespace1::externaltests::GlobalEnums;
    SimpleExternalEnumMessage test;
    int propertyNumber = SimpleExternalEnumMessage::propertyOrdering.at(1); //See externalpackagetest.proto
    ASSERT_STREQ(SimpleExternalEnumMessage::staticMetaObject.property(propertyNumber).typeName(), "qtprotobufnamespace1::externaltests::GlobalEnums::ExternalTestEnum");
    ASSERT_EQ(SimpleExternalEnumMessage::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<ExternalGlobalEnums::ExternalTestEnum>());
    ASSERT_STREQ(SimpleExternalEnumMessage::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<ExternalGlobalEnums::ExternalTestEnum>(ExternalGlobalEnums::EXTERNAL_TEST_ENUM_VALUE4)));
    ASSERT_TRUE(test.property(propertyName).value<ExternalGlobalEnums::ExternalTestEnum>() == QVariant::fromValue<ExternalGlobalEnums::ExternalTestEnum>(ExternalGlobalEnums::EXTERNAL_TEST_ENUM_VALUE4));
    ASSERT_TRUE(test.externalEnum() == QVariant::fromValue<ExternalGlobalEnums::ExternalTestEnum>(ExternalGlobalEnums::EXTERNAL_TEST_ENUM_VALUE4));
}

TEST_F(SimpleTest, SimpleEnumsTest)
{
    ASSERT_GT(GlobalEnums::staticMetaObject.enumeratorCount(), 0);
    QMetaEnum simpleEnum;
    for (int i = 0; i < GlobalEnums::staticMetaObject.enumeratorCount(); i++) {
        QMetaEnum tmp = GlobalEnums::staticMetaObject.enumerator(i);
        if (QString(tmp.name()) == QString("TestEnum")) {
            simpleEnum = tmp;
            break;
        }
    }
    ASSERT_TRUE(simpleEnum.isValid());
    ASSERT_STREQ(simpleEnum.key(0), "TEST_ENUM_VALUE0");
    ASSERT_STREQ(simpleEnum.key(1), "TEST_ENUM_VALUE1");
    ASSERT_STREQ(simpleEnum.key(2), "TEST_ENUM_VALUE2");
    ASSERT_STREQ(simpleEnum.key(3), "TEST_ENUM_VALUE3");
    ASSERT_STREQ(simpleEnum.key(4), "TEST_ENUM_VALUE4");

    ASSERT_EQ(simpleEnum.value(0), 0);
    ASSERT_EQ(simpleEnum.value(1), 1);
    ASSERT_EQ(simpleEnum.value(2), 2);
    ASSERT_EQ(simpleEnum.value(3), 4);
    ASSERT_EQ(simpleEnum.value(4), 3);
}

TEST_F(SimpleTest, SimpleFileEnumsTest)
{
    const char* propertyName = "globalEnumList";
    SimpleFileEnumMessage test;
    int propertyNumber = SimpleFileEnumMessage::propertyOrdering.at(2); //See simpletest.proto
    ASSERT_STREQ(SimpleFileEnumMessage::staticMetaObject.property(propertyNumber).typeName(), "GlobalEnums::TestEnumList");
    ASSERT_EQ(SimpleFileEnumMessage::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<GlobalEnums::TestEnumList>());
    ASSERT_STREQ(SimpleFileEnumMessage::staticMetaObject.property(propertyNumber).name(), propertyName);

    GlobalEnums::TestEnumList value({GlobalEnums::TEST_ENUM_VALUE1,
                                     GlobalEnums::TEST_ENUM_VALUE3,
                                     GlobalEnums::TEST_ENUM_VALUE4,
                                     GlobalEnums::TEST_ENUM_VALUE2,
                                     GlobalEnums::TEST_ENUM_VALUE1});
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<GlobalEnums::TestEnumList>(value)));
    ASSERT_TRUE(test.property(propertyName).value<GlobalEnums::TestEnumList>() == value);
    ASSERT_TRUE(test.globalEnumList() == value);
}

TEST_F(SimpleTest, ComplexMessageTest)
{
    ComplexMessage msg;
}

TEST_F(SimpleTest, SimpleBytesMessageTest)
{
    const char* propertyName = "testFieldBytes";
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
    const char* propertyName = "localList";
    qtprotobufnamespace1::externaltests::SimpleExternalMessage test;
    int propertyNumber = qtprotobufnamespace1::externaltests::SimpleExternalMessage::propertyOrdering.at(1);
    ASSERT_STREQ(qtprotobufnamespace1::externaltests::SimpleExternalMessage::staticMetaObject.property(propertyNumber).typeName(), "qtprotobuf::int32List");
    ASSERT_EQ(qtprotobufnamespace1::externaltests::SimpleExternalMessage::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<qtprotobuf::int32List>());
    ASSERT_STREQ(qtprotobufnamespace1::externaltests::SimpleExternalMessage::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<int32List>({1, 2, 3, 4, 5})));
    ASSERT_TRUE(test.property(propertyName).value<int32List>() == int32List({1, 2, 3, 4, 5}));
    ASSERT_TRUE(test.localList() == int32List({1, 2, 3, 4, 5}));
}

TEST_F(SimpleTest, RepeatedExternalComplexMessageTest)
{
    const char* propertyName = "testExternalComplex";
    RepeatedExternalComplexMessage test;
    int propertyNumber = RepeatedExternalComplexMessage::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_STREQ(RepeatedExternalComplexMessage::staticMetaObject.property(propertyNumber).typeName(), "qtprotobufnamespace1::externaltests::ExternalComplexMessageList");
    ASSERT_EQ(RepeatedExternalComplexMessage::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<qtprotobufnamespace1::externaltests::ExternalComplexMessageList>());
    ASSERT_STREQ(RepeatedExternalComplexMessage::staticMetaObject.property(propertyNumber).name(), propertyName);

    qtprotobufnamespace1::externaltests::SimpleExternalMessage complexMessage;
    complexMessage.setLocalList({1, 2, 3, 4, 5});

    qtprotobufnamespace1::externaltests::ExternalComplexMessage externalMessage;
    externalMessage.setTestFieldInt(complexMessage);

    qtprotobufnamespace1::externaltests::ExternalComplexMessageList complexMessageList;
    complexMessageList << externalMessage;

    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue(complexMessageList)));
    ASSERT_TRUE(test.property(propertyName).value<qtprotobufnamespace1::externaltests::ExternalComplexMessageList>() == complexMessageList);
    ASSERT_TRUE(test.testExternalComplex() == complexMessageList);
}

TEST_F(SimpleTest, RepeatedStringMessageTest)
{
    const char* propertyName = "testRepeatedString";
    RepeatedStringMessage test;
    int propertyNumber = RepeatedStringMessage::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_STREQ(RepeatedStringMessage::staticMetaObject.property(propertyNumber).typeName(), "QStringList");
    ASSERT_EQ(RepeatedStringMessage::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<QStringList>());
    ASSERT_STREQ(RepeatedStringMessage::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<QStringList>({"Text", "tryam"})));
    ASSERT_TRUE(test.property(propertyName).value<QStringList>() == QStringList({"Text", "tryam"}));
    ASSERT_TRUE(test.testRepeatedString() == QStringList({"Text", "tryam"}));
}

TEST_F(SimpleTest, RepeatedIntMessageTest)
{
    const char* propertyName = "testRepeatedInt";
    RepeatedIntMessage test;
    int propertyNumber = RepeatedIntMessage::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_STREQ(RepeatedIntMessage::staticMetaObject.property(propertyNumber).typeName(), "qtprotobuf::int32List");
    ASSERT_EQ(RepeatedIntMessage::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<qtprotobuf::int32List>());
    ASSERT_STREQ(RepeatedIntMessage::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<int32List>({1, 2, 3, 4, 5})));
    ASSERT_TRUE(test.property(propertyName).value<int32List>() == int32List({1, 2, 3, 4, 5}));
    ASSERT_TRUE(test.testRepeatedInt() == int32List({1, 2, 3, 4, 5}));
}

TEST_F(SimpleTest, RepeatedDoubleMessageTest)
{
    const char* propertyName = "testRepeatedDouble";
    RepeatedDoubleMessage test;
    int propertyNumber = RepeatedDoubleMessage::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_STREQ(RepeatedDoubleMessage::staticMetaObject.property(propertyNumber).typeName(), "qtprotobuf::DoubleList");
    ASSERT_EQ(RepeatedDoubleMessage::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<qtprotobuf::DoubleList>());
    ASSERT_STREQ(RepeatedDoubleMessage::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<qtprotobuf::DoubleList>({1.0, 2.3, 3, 4.7, 5.9})));
    ASSERT_TRUE(test.property(propertyName).value<qtprotobuf::DoubleList>() == qtprotobuf::DoubleList({1.0, 2.3, 3, 4.7, 5.9}));
    ASSERT_TRUE(test.testRepeatedDouble() == qtprotobuf::DoubleList({1.0, 2.3, 3, 4.7, 5.9}));
}

TEST_F(SimpleTest, RepeatedFloatMessageTest)
{
    const char* propertyName = "testRepeatedFloat";
    RepeatedFloatMessage test;
    int propertyNumber = RepeatedFloatMessage::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_STREQ(RepeatedFloatMessage::staticMetaObject.property(propertyNumber).typeName(), "qtprotobuf::FloatList");
    ASSERT_EQ(RepeatedFloatMessage::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<qtprotobuf::FloatList>());
    ASSERT_STREQ(RepeatedFloatMessage::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<qtprotobuf::FloatList>({1.0f, 2.3f, 3, 4.7f, 5.9f})));
    ASSERT_TRUE(test.property(propertyName).value<qtprotobuf::FloatList>() == qtprotobuf::FloatList({1.0f, 2.3f, 3, 4.7f, 5.9f}));
    ASSERT_TRUE(test.testRepeatedFloat() == qtprotobuf::FloatList({1.0f, 2.3f, 3, 4.7f, 5.9f}));
}

TEST_F(SimpleTest, RepeatedBytesMessageTest)
{
    const char* propertyName = "testRepeatedBytes";
    RepeatedBytesMessage test;
    int propertyNumber = RepeatedBytesMessage::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_STREQ(RepeatedBytesMessage::staticMetaObject.property(propertyNumber).typeName(), "QByteArrayList");
    ASSERT_EQ(RepeatedBytesMessage::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<QByteArrayList>());
    ASSERT_STREQ(RepeatedBytesMessage::staticMetaObject.property(propertyNumber).name(), propertyName);
    QByteArrayList bList;
    bList << "\x01\x02\x03\x04\x05";
    bList << "\x01\x05\x03\x04\x03";

    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<QByteArrayList>(bList)));
    ASSERT_TRUE(test.property(propertyName).value<QByteArrayList>() == bList);
    ASSERT_TRUE(test.testRepeatedBytes() == bList);
}

TEST_F(SimpleTest, RepeatedSIntMessageTest)
{
    const char* propertyName = "testRepeatedInt";
    RepeatedSIntMessage test;
    int propertyNumber = RepeatedSIntMessage::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_STREQ(RepeatedSIntMessage::staticMetaObject.property(propertyNumber).typeName(), "qtprotobuf::sint32List");
    ASSERT_EQ(RepeatedSIntMessage::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<qtprotobuf::sint32List>());
    ASSERT_STREQ(RepeatedSIntMessage::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<sint32List>({1, 2, 3, 4, 5})));
    ASSERT_TRUE(test.property(propertyName).value<sint32List>() == sint32List({1, 2, 3, 4, 5}));
    ASSERT_TRUE(test.testRepeatedInt() == sint32List({1, 2, 3, 4, 5}));
}

TEST_F(SimpleTest, RepeatedUIntMessageTest)
{
    const char* propertyName = "testRepeatedInt";
    RepeatedUIntMessage test;
    int propertyNumber = RepeatedUIntMessage::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_STREQ(RepeatedUIntMessage::staticMetaObject.property(propertyNumber).typeName(), "qtprotobuf::uint32List");
    ASSERT_EQ(RepeatedUIntMessage::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<qtprotobuf::uint32List>());
    ASSERT_STREQ(RepeatedUIntMessage::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<uint32List>({1, 2, 3, 4, 5})));
    ASSERT_TRUE(test.property(propertyName).value<uint32List>() == uint32List({1, 2, 3, 4, 5}));
    ASSERT_TRUE(test.testRepeatedInt() == uint32List({1, 2, 3, 4, 5}));
}

TEST_F(SimpleTest, RepeatedInt64MessageTest)
{
    const char* propertyName = "testRepeatedInt";
    RepeatedInt64Message test;
    int propertyNumber = RepeatedInt64Message::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_STREQ(RepeatedInt64Message::staticMetaObject.property(propertyNumber).typeName(), "qtprotobuf::int64List");
    ASSERT_EQ(RepeatedInt64Message::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<qtprotobuf::int64List>());
    ASSERT_STREQ(RepeatedInt64Message::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<int64List>({1, 2, 3, 4, 5})));
    ASSERT_TRUE(test.property(propertyName).value<int64List>() == int64List({1, 2, 3, 4, 5}));
    ASSERT_TRUE(test.testRepeatedInt() == int64List({1, 2, 3, 4, 5}));
}

TEST_F(SimpleTest, RepeatedSInt64MessageTest)
{
    const char* propertyName = "testRepeatedInt";
    RepeatedSInt64Message test;
    int propertyNumber = RepeatedSInt64Message::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_STREQ(RepeatedSInt64Message::staticMetaObject.property(propertyNumber).typeName(), "qtprotobuf::sint64List");
    ASSERT_EQ(RepeatedSInt64Message::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<qtprotobuf::sint64List>());
    ASSERT_STREQ(RepeatedSInt64Message::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<sint64List>({1, 2, 3, 4, 5})));
    ASSERT_TRUE(test.property(propertyName).value<sint64List>() == sint64List({1, 2, 3, 4, 5}));
    ASSERT_TRUE(test.testRepeatedInt() == sint64List({1, 2, 3, 4, 5}));
}

TEST_F(SimpleTest, RepeatedUInt64MessageTest)
{
    const char* propertyName = "testRepeatedInt";
    RepeatedUInt64Message test;
    int propertyNumber = RepeatedUInt64Message::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_STREQ(RepeatedUInt64Message::staticMetaObject.property(propertyNumber).typeName(), "qtprotobuf::uint64List");
    ASSERT_EQ(RepeatedUInt64Message::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<qtprotobuf::uint64List>());
    ASSERT_STREQ(RepeatedUInt64Message::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<uint64List>({1, 2, 3, 4, 5})));
    ASSERT_TRUE(test.property(propertyName).value<uint64List>() == uint64List({1, 2, 3, 4, 5}));
    ASSERT_TRUE(test.testRepeatedInt() == uint64List({1, 2, 3, 4, 5}));
}

TEST_F(SimpleTest, RepeatedFixedIntMessageTest)
{
    const char* propertyName = "testRepeatedInt";
    RepeatedFixedIntMessage test;
    int propertyNumber = RepeatedFixedIntMessage::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_STREQ(RepeatedFixedIntMessage::staticMetaObject.property(propertyNumber).typeName(), "qtprotobuf::fint32List");
    ASSERT_EQ(RepeatedFixedIntMessage::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<qtprotobuf::fint32List>());
    ASSERT_STREQ(RepeatedFixedIntMessage::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<fint32List>({1, 2, 3, 4, 5})));
    ASSERT_TRUE(test.property(propertyName).value<fint32List>() == fint32List({1, 2, 3, 4, 5}));
    ASSERT_TRUE(test.testRepeatedInt() == fint32List({1, 2, 3, 4, 5}));
}

TEST_F(SimpleTest, RepeatedFixedInt64MessageTest)
{
    const char* propertyName = "testRepeatedInt";
    RepeatedFixedInt64Message test;
    int propertyNumber = RepeatedFixedInt64Message::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_STREQ(RepeatedFixedInt64Message::staticMetaObject.property(propertyNumber).typeName(), "qtprotobuf::fint64List");
    ASSERT_EQ(RepeatedFixedInt64Message::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<qtprotobuf::fint64List>());
    ASSERT_STREQ(RepeatedFixedInt64Message::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<fint64List>({1, 2, 3, 4, 5})));
    ASSERT_TRUE(test.property(propertyName).value<fint64List>() == fint64List({1, 2, 3, 4, 5}));
    ASSERT_TRUE(test.testRepeatedInt() == fint64List({1, 2, 3, 4, 5}));
}

TEST_F(SimpleTest, RepeatedSFixedIntMessageTest)
{
    const char* propertyName = "testRepeatedInt";
    RepeatedSFixedIntMessage test;
    int propertyNumber = RepeatedSFixedIntMessage::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_STREQ(RepeatedSFixedIntMessage::staticMetaObject.property(propertyNumber).typeName(), "qtprotobuf::sfint32List");
    ASSERT_EQ(RepeatedSFixedIntMessage::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<qtprotobuf::sfint32List>());
    ASSERT_STREQ(RepeatedSFixedIntMessage::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<sfint32List>({1, 2, 3, 4, 5})));
    ASSERT_TRUE(test.property(propertyName).value<sfint32List>() == sfint32List({1, 2, 3, 4, 5}));
    ASSERT_TRUE(test.testRepeatedInt() == sfint32List({1, 2, 3, 4, 5}));
}

TEST_F(SimpleTest, RepeatedSFixedInt64MessageTest)
{
    const char* propertyName = "testRepeatedInt";
    RepeatedSFixedInt64Message test;
    int propertyNumber = RepeatedSFixedInt64Message::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_STREQ(RepeatedSFixedInt64Message::staticMetaObject.property(propertyNumber).typeName(), "qtprotobuf::sfint64List");
    ASSERT_EQ(RepeatedSFixedInt64Message::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<qtprotobuf::sfint64List>());
    ASSERT_STREQ(RepeatedSFixedInt64Message::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<sfint64List>({1, 2, 3, 4, 5})));
    ASSERT_TRUE(test.property(propertyName).value<sfint64List>() == sfint64List({1, 2, 3, 4, 5}));
    ASSERT_TRUE(test.testRepeatedInt() == sfint64List({1, 2, 3, 4, 5}));
}

TEST_F(SimpleTest, StepChildEnumMessageTest)
{
    const char* propertyName = "localStepChildEnum";
    StepChildEnumMessage test;
    int propertyNumber = StepChildEnumMessage::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_STREQ(StepChildEnumMessage::staticMetaObject.property(propertyNumber).typeName(), "qtprotobufnamespace::tests::SimpleEnumMessage::LocalEnum");
    ASSERT_EQ(StepChildEnumMessage::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<SimpleEnumMessage::LocalEnum>());
    ASSERT_STREQ(StepChildEnumMessage::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<qtprotobufnamespace::tests::SimpleEnumMessage::LocalEnum>(SimpleEnumMessage::LOCAL_ENUM_VALUE2)));
    ASSERT_TRUE(test.property(propertyName).value<SimpleEnumMessage::LocalEnum>() == SimpleEnumMessage::LOCAL_ENUM_VALUE2);
    ASSERT_TRUE(test.localStepChildEnum() == SimpleEnumMessage::LOCAL_ENUM_VALUE2);
}

TEST_F(SimpleTest, StepChildEnumListMessageTest)
{
    const char* propertyName = "localStepChildList";
    StepChildEnumMessage test;
    int propertyNumber = StepChildEnumMessage::propertyOrdering.at(2); //See simpletest.proto
    ASSERT_STREQ(StepChildEnumMessage::staticMetaObject.property(propertyNumber).typeName(), "qtprotobufnamespace::tests::SimpleEnumMessage::LocalEnumList");
    ASSERT_EQ(StepChildEnumMessage::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<qtprotobufnamespace::tests::SimpleEnumMessage::LocalEnumList>());
    ASSERT_STREQ(StepChildEnumMessage::staticMetaObject.property(propertyNumber).name(), propertyName);

    qtprotobufnamespace::tests::SimpleEnumMessage::LocalEnumList value({SimpleEnumMessage::LOCAL_ENUM_VALUE2,
                                                                        SimpleEnumMessage::LOCAL_ENUM_VALUE2,
                                                                        SimpleEnumMessage::LOCAL_ENUM_VALUE1,
                                                                        SimpleEnumMessage::LOCAL_ENUM_VALUE3});
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<qtprotobufnamespace::tests::SimpleEnumMessage::LocalEnumList>(value)));
    ASSERT_TRUE(test.property(propertyName).value<SimpleEnumMessage::LocalEnumList>() == value);
    ASSERT_TRUE(test.localStepChildList() == value);
}


TEST_F(SimpleTest, SimpleSInt32MapMessage)
{
    const char* propertyName = "mapField";
    SimpleSInt32MapMessage::registerTypes();
    SimpleSInt32MapMessage test;
    ASSERT_TRUE(QMetaType::isRegistered(qMetaTypeId<SimpleSInt32MapMessage::MapFieldEntry>()));
    int propertyNumber = SimpleSInt32MapMessage::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_STREQ(SimpleSInt32MapMessage::staticMetaObject.property(propertyNumber).typeName(), "SimpleSInt32MapMessage::MapFieldEntry");
    ASSERT_EQ(SimpleSInt32MapMessage::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<SimpleSInt32MapMessage::MapFieldEntry>());
    ASSERT_STREQ(SimpleSInt32MapMessage::staticMetaObject.property(propertyNumber).name(), propertyName);
    SimpleSInt32MapMessage::MapFieldEntry testMap = {{10, {"Some 10"}}, {0, {"Some 0"}}, {44, {"Some 44"}}};
    test.setMapField(testMap);
    ASSERT_TRUE(test.property(propertyName).value<SimpleSInt32MapMessage::MapFieldEntry>() == testMap);
    ASSERT_TRUE(test.mapField() == testMap);
    ASSERT_STREQ(test.mapField()[10].testFieldString().toStdString().c_str(), "Some 10");
    ASSERT_STREQ(test.mapField()[0].testFieldString().toStdString().c_str(), "Some 0");
    ASSERT_STREQ(test.mapField()[44].testFieldString().toStdString().c_str(), "Some 44");
}

TEST_F(SimpleTest, SimpleStringMapMessage)
{
    const char* propertyName = "mapField";
    SimpleStringMapMessage::registerTypes();
    SimpleStringMapMessage test;
    ASSERT_TRUE(QMetaType::isRegistered(qMetaTypeId<SimpleStringMapMessage::MapFieldEntry>()));
    int propertyNumber = SimpleStringMapMessage::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_STREQ(SimpleStringMapMessage::staticMetaObject.property(propertyNumber).typeName(), "SimpleStringMapMessage::MapFieldEntry");
    ASSERT_EQ(SimpleStringMapMessage::staticMetaObject.property(propertyNumber).userType(), qMetaTypeId<SimpleStringMapMessage::MapFieldEntry>());
    ASSERT_STREQ(SimpleStringMapMessage::staticMetaObject.property(propertyNumber).name(), propertyName);
    SimpleStringMapMessage::MapFieldEntry testMap = {{"key 10", {"Some 10", nullptr}}, {"key 0", {"Some 0", nullptr}}, {"key 44", {"Some 44", nullptr}}};
    test.setMapField(testMap);
    ASSERT_TRUE(test.property(propertyName).value<SimpleStringMapMessage::MapFieldEntry>() == testMap);
    ASSERT_TRUE(test.mapField() == testMap);
    ASSERT_STREQ(test.mapField()["key 10"].testFieldString().toStdString().c_str(), "Some 10");
    ASSERT_STREQ(test.mapField()["key 0"].testFieldString().toStdString().c_str(), "Some 0");
    ASSERT_STREQ(test.mapField()["key 44"].testFieldString().toStdString().c_str(), "Some 44");
}
