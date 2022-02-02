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

#include "extra_namespace.qpb.h"

#include <QVariantList>
#include <QMetaProperty>
#include <QSignalSpy>

#include <gtest/gtest.h>
#include <qtprotobuftestscommon.h>

using namespace MyTestNamespace::qtprotobufnamespace::tests;

class ExtraNamespaceTest : public ::testing::Test
{
public:
    // see simpletest.proto for property names and their field indices
    ExtraNamespaceTest()
    {
    }

    static void SetUpTestCase();
};

void ExtraNamespaceTest::SetUpTestCase()
{
    qRegisterProtobufTypes();
}

TEST_F(ExtraNamespaceTest, EmptyMessageTest)
{
    ASSERT_EQ(MyTestNamespace::qtprotobufnamespace::tests::EmptyMessage::propertyOrdering.size(), 0);
    ASSERT_EQ(MyTestNamespace::qtprotobufnamespace::tests::EmptyMessage::staticMetaObject.propertyCount(), 1);
}

TEST_F(ExtraNamespaceTest, ComplexMessageTest)
{
    const char *propertyName = "testComplexField";
    qProtobufAssertMessagePropertyRegistered<MyTestNamespace::qtprotobufnamespace::tests::ComplexMessage, MyTestNamespace::qtprotobufnamespace::tests::SimpleStringMessage*>(
                2, "MyTestNamespace::qtprotobufnamespace::tests::SimpleStringMessage*", propertyName);

    ComplexMessage test;
    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<MyTestNamespace::qtprotobufnamespace::tests::SimpleStringMessage*>(new MyTestNamespace::qtprotobufnamespace::tests::SimpleStringMessage{"test qwerty"})));
    ASSERT_TRUE(*(test.property(propertyName).value<MyTestNamespace::qtprotobufnamespace::tests::SimpleStringMessage*>()) == MyTestNamespace::qtprotobufnamespace::tests::SimpleStringMessage{"test qwerty"});
    ASSERT_TRUE(test.testComplexField() == MyTestNamespace::qtprotobufnamespace::tests::SimpleStringMessage{"test qwerty"});
}
