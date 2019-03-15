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

#include "simpleintmessage.h"
#include "simplefixedint32message.h"
#include "simplefixedint64message.h"
#include "simplestringmessage.h"
#include "simplefloatmessage.h"
#include "simpledoublemessage.h"
#include "simpleenummessage.h"
#include "complexmessage.h"
#include "repeatedintmessage.h"
#include "simplebytesmessage.h"
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

TEST_F(SimpleTest, SimpleIntMessageTest)
{
    const char* propertyName = "testFieldInt";
    SimpleIntMessage test;
    int propertyNumber = SimpleIntMessage::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_EQ(SimpleIntMessage::staticMetaObject.property(propertyNumber).type(), QMetaType::Int);
    ASSERT_STREQ(SimpleIntMessage::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue(1)));
    ASSERT_EQ(test.property(propertyName).toInt(), 1);
    ASSERT_EQ(test.testFieldInt(), 1);
}

TEST_F(SimpleTest, SimpleFixedInt32MessageTest)
{
    const char* propertyName = "testFieldFixedInt32";
    SimpleFixedInt32Message test;
    int propertyNumber = SimpleFixedInt32Message::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_EQ(SimpleFixedInt32Message::staticMetaObject.property(propertyNumber).type(), qMetaTypeId<FixedInt32>());
    ASSERT_STREQ(SimpleFixedInt32Message::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue(1)));
    ASSERT_EQ(test.property(propertyName).value<FixedInt32>(), 1);
    ASSERT_EQ(test.testFieldFixedInt32(), 1);
}

TEST_F(SimpleTest, SimpleFixedInt64MessageTest)
{
    const char* propertyName = "testFieldFixedInt64";
    SimpleFixedInt64Message test;
    int propertyNumber = SimpleFixedInt64Message::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_EQ(SimpleFixedInt64Message::staticMetaObject.property(propertyNumber).type(), qMetaTypeId<FixedInt64>());
    ASSERT_STREQ(SimpleFixedInt64Message::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue(1)));
    ASSERT_EQ(test.property(propertyName).value<FixedInt64>(), 1);
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

TEST_F(SimpleTest, SimpleLocalEnumsTest)
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

TEST_F(SimpleTest, RepeatedIntMessageTest)
{
    const char* propertyName = "testRepeatedInt";
    RepeatedIntMessage test;
    int propertyNumber = RepeatedIntMessage::propertyOrdering.at(1); //See simpletest.proto
    ASSERT_EQ(RepeatedIntMessage::staticMetaObject.property(propertyNumber).type(), qMetaTypeId<IntList>());
    ASSERT_STREQ(RepeatedIntMessage::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<IntList>({1,2,3,4,5})));
    ASSERT_TRUE(test.property(propertyName).value<IntList>() == IntList({1, 2, 3, 4, 5}));
    ASSERT_TRUE(test.testRepeatedInt() == IntList({1, 2, 3, 4, 5}));
}
