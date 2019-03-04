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
#include "simplestringmessage.h"
#include "simplefloatmessage.h"
#include "simpledoublemessage.h"
#include <QMetaProperty>

using namespace qtprotobuf::tests;


SimpleTest::SimpleTest()
{
}

TEST_F(SimpleTest, SimpleIntMessageTest)
{
    const char* propertyName = "testFieldInt";
    SimpleIntMessage test;
    int propertyNumber = SimpleIntMessage::propertyOrdering.at(5); //See simpletest.proto
    ASSERT_EQ(SimpleIntMessage::staticMetaObject.property(propertyNumber).type(), QVariant::Int);
    ASSERT_STREQ(SimpleIntMessage::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue(1)));
    ASSERT_EQ(test.property(propertyName).toInt(), 1);
}

TEST_F(SimpleTest, SimpleStringMessageTest)
{
    const char* propertyName = "testFieldString";
    SimpleStringMessage test;
    int propertyNumber = SimpleStringMessage::propertyOrdering.at(6); //See simpletest.proto
    ASSERT_EQ(SimpleStringMessage::staticMetaObject.property(propertyNumber).type(), QVariant::String);
    ASSERT_STREQ(SimpleStringMessage::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue(QString("test1"))));
    ASSERT_STREQ(test.property(propertyName).toString().toStdString().c_str(), "test1");
}

TEST_F(SimpleTest, SimpleFloatMessageTest)
{
    const char* propertyName = "testFieldFloat";
    SimpleFloatMessage test;
    int propertyNumber = SimpleFloatMessage::propertyOrdering.at(7); //See simpletest.proto
    ASSERT_EQ(SimpleFloatMessage::staticMetaObject.property(propertyNumber).type(), QVariant::Double);
    ASSERT_STREQ(SimpleFloatMessage::staticMetaObject.property(propertyNumber).name(), "testFieldFloat");
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue(1.55)));
    ASSERT_EQ(test.property(propertyName).toDouble(), 1.55);
}

TEST_F(SimpleTest, SimpleDoubleMessageTest)
{
    const char* propertyName = "testFieldDouble";
    SimpleDoubleMessage test;
    int propertyNumber = SimpleDoubleMessage::propertyOrdering.at(8); //See simpletest.proto
    ASSERT_EQ(SimpleDoubleMessage::staticMetaObject.property(propertyNumber).type(), QVariant::Double);
    ASSERT_STREQ(SimpleDoubleMessage::staticMetaObject.property(propertyNumber).name(), propertyName);
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue(0.55)));
    ASSERT_EQ(test.property(propertyName).toDouble(), 0.55);
}

