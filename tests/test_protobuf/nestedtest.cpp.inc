/*
 * MIT License
 *
 * Copyright (c) 2020 Alexey Edelev <semlanik@gmail.com>
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

#include <gtest/gtest.h>
#include <qprotobufserializer.h>
#include "../testscommon.h"

using namespace qtprotobufnamespace::tests::nested;

namespace QtProtobuf {
namespace tests {

class NestedTest : public ::testing::Test
{
public:
    NestedTest() = default;
    void SetUp() override;
    static void SetUpTestCase();
protected:
    std::unique_ptr<QProtobufSerializer> serializer;
};


void NestedTest::SetUpTestCase()
{
    //Register all types
    QtProtobuf::qRegisterProtobufTypes();
}

void NestedTest::SetUp()
{
    serializer.reset(new QProtobufSerializer);
}

TEST_F(NestedTest, NestedMessageTest)
{
    assertMessagePropertyRegistered<NestedFieldMessage::NestedMessage, QtProtobuf::sint32>(1, "QtProtobuf::sint32", "testFieldInt");
}

TEST_F(NestedTest, SimpleTest)
{
    const char *propertyName = "nested";

    assertMessagePropertyRegistered<NestedFieldMessage, NestedFieldMessage::NestedMessage*>(2, "qtprotobufnamespace::tests::nested::NestedFieldMessage::NestedMessage*", "nested");

    NestedFieldMessage test{10, {15}};
    EXPECT_EQ(test.nested().testFieldInt(), 15);

    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<NestedFieldMessage::NestedMessage*>(new NestedFieldMessage::NestedMessage{55})));
    ASSERT_TRUE(*(test.property(propertyName).value<NestedFieldMessage::NestedMessage*>()) == NestedFieldMessage::NestedMessage{55});
    ASSERT_TRUE(test.nested() == NestedFieldMessage::NestedMessage{55});

    assertMessagePropertyRegistered<NestedFieldMessage2::NestedMessageLevel1, NestedFieldMessage2::NestedMessageLevel1::NestedMessageLevel2*>(1, "qtprotobufnamespace::tests::nested::NestedFieldMessage2::NestedMessageLevel1::NestedMessageLevel2*", "nested");

    NestedFieldMessage2::NestedMessageLevel1 level1{{20}};
    EXPECT_EQ(level1.nested().testFieldInt(), 20);
    ASSERT_TRUE(level1.setProperty(propertyName, QVariant::fromValue<NestedFieldMessage2::NestedMessageLevel1::NestedMessageLevel2*>(new NestedFieldMessage2::NestedMessageLevel1::NestedMessageLevel2{55})));
    ASSERT_TRUE(*(level1.property(propertyName).value<NestedFieldMessage2::NestedMessageLevel1::NestedMessageLevel2*>()) == NestedFieldMessage2::NestedMessageLevel1::NestedMessageLevel2{55});
    ASSERT_TRUE(level1.nested() == NestedFieldMessage2::NestedMessageLevel1::NestedMessageLevel2{55});

    assertMessagePropertyRegistered<NestedFieldMessage2, NestedFieldMessage2::NestedMessageLevel1::NestedMessageLevel2*>(3, "qtprotobufnamespace::tests::nested::NestedFieldMessage2::NestedMessageLevel1::NestedMessageLevel2*", "nested2");

    NestedFieldMessage2 test2{level1, {20}};
    EXPECT_EQ(test2.nested1().nested().testFieldInt(), 55);
    EXPECT_EQ(test2.nested2().testFieldInt(), 20);

    ASSERT_TRUE(test2.setProperty("nested1", QVariant::fromValue<NestedFieldMessage2::NestedMessageLevel1*>(new NestedFieldMessage2::NestedMessageLevel1{{65}})));
    ASSERT_TRUE(*(test2.property("nested1").value<NestedFieldMessage2::NestedMessageLevel1*>()) == NestedFieldMessage2::NestedMessageLevel1{{65}});
    ASSERT_TRUE(test2.nested1() == NestedFieldMessage2::NestedMessageLevel1{{65}});

    ASSERT_TRUE(test2.setProperty("nested2", QVariant::fromValue<NestedFieldMessage2::NestedMessageLevel1::NestedMessageLevel2*>(new NestedFieldMessage2::NestedMessageLevel1::NestedMessageLevel2{75})));
    ASSERT_TRUE(*(test2.property("nested2").value<NestedFieldMessage2::NestedMessageLevel1::NestedMessageLevel2*>()) == NestedFieldMessage2::NestedMessageLevel1::NestedMessageLevel2{75});
    ASSERT_TRUE(test2.nested2() == NestedFieldMessage2::NestedMessageLevel1::NestedMessageLevel2{75});
}

TEST_F(NestedTest, SerializationTest)
{
    NestedFieldMessage::NestedMessage nested{15};
    QByteArray result = nested.serialize(serializer.get());
    EXPECT_EQ(result.size(), 2);
    EXPECT_STREQ(result.toHex().toStdString().c_str(), "081e");

    NestedFieldMessage test{10, nested};
    result = test.serialize(serializer.get());
    EXPECT_TRUE(result == QByteArray::fromHex("1202081e0814")
                || result == QByteArray::fromHex("08141202081e"));

    NestedFieldMessage2::NestedMessageLevel1 level1{{10}};
    result = level1.serialize(serializer.get());
    EXPECT_STREQ(result.toHex().toStdString().c_str(), "0a020814");

    NestedFieldMessage2 test2{level1, {15}};
    result = test2.serialize(serializer.get());
    EXPECT_TRUE(result == QByteArray::fromHex("1a02081e12040a020814")
                || result == QByteArray::fromHex("12040a0208141a02081e"));

    NeighborNested neigbor{{15},{20}};
    result = neigbor.serialize(serializer.get());
    EXPECT_TRUE(result == QByteArray::fromHex("120208280a02081e")
                || result == QByteArray::fromHex("0a02081e12020828"));
}

TEST_F(NestedTest, DeserializationTest)
{
    NestedFieldMessage::NestedMessage nested;

    nested.deserialize(serializer.get(), QByteArray::fromHex("081e"));
    EXPECT_EQ(nested.testFieldInt(), 15);

    NestedFieldMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("1202081e0814"));
    EXPECT_EQ(test.nested().testFieldInt(), 15);

    NestedFieldMessage2::NestedMessageLevel1 level1;
    level1.deserialize(serializer.get(), QByteArray::fromHex("0a020814"));
    EXPECT_EQ(level1.nested().testFieldInt(), 10);

    NestedFieldMessage2 test2;
    test2.deserialize(serializer.get(), QByteArray::fromHex("1a02081e12040a020814"));
    EXPECT_EQ(test2.nested1().nested().testFieldInt(), 10);
    EXPECT_EQ(test2.nested2().testFieldInt(), 15);

    NeighborNested neigbor;
    neigbor.deserialize(serializer.get(), QByteArray::fromHex("120208280a02081e"));
    EXPECT_EQ(neigbor.neighborNested().testFieldInt(), 15);
    EXPECT_EQ(neigbor.neighborNested2().testFieldInt(), 20);
}


TEST_F(NestedTest, NeighborTest)
{
    assertMessagePropertyRegistered<NeighborNested, NestedFieldMessage::NestedMessage*>(1, "qtprotobufnamespace::tests::nested::NestedFieldMessage::NestedMessage*", "neighborNested");
    assertMessagePropertyRegistered<NeighborNested, NestedFieldMessage2::NestedMessageLevel1::NestedMessageLevel2*>(2, "qtprotobufnamespace::tests::nested::NestedFieldMessage2::NestedMessageLevel1::NestedMessageLevel2*", "neighborNested2");

    NeighborNested test{{15},{20}};
    EXPECT_EQ(test.neighborNested().testFieldInt(), 15);
    EXPECT_EQ(test.neighborNested2().testFieldInt(), 20);

    const char *propertyName = "neighborNested";

    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<NestedFieldMessage::NestedMessage*>(new NestedFieldMessage::NestedMessage{55})));
    ASSERT_TRUE(*(test.property(propertyName).value<NestedFieldMessage::NestedMessage*>()) == NestedFieldMessage::NestedMessage{55});
    ASSERT_TRUE(test.neighborNested() == NestedFieldMessage::NestedMessage{55});

    propertyName = "neighborNested2";

    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<NestedFieldMessage2::NestedMessageLevel1::NestedMessageLevel2*>(new NestedFieldMessage2::NestedMessageLevel1::NestedMessageLevel2{75})));
    ASSERT_TRUE(*(test.property(propertyName).value<NestedFieldMessage2::NestedMessageLevel1::NestedMessageLevel2*>()) == NestedFieldMessage2::NestedMessageLevel1::NestedMessageLevel2{75});
    ASSERT_TRUE(test.neighborNested2() == NestedFieldMessage2::NestedMessageLevel1::NestedMessageLevel2{75});
}

TEST_F(NestedTest, ExternalTest)
{
    assertMessagePropertyRegistered<NestedExternal, qtprotobufnamespace1::externaltests::NestedFieldMessage::NestedMessage*>(1, "qtprotobufnamespace1::externaltests::NestedFieldMessage::NestedMessage*", "externalNested");

    NestedExternal test{{15}};
    EXPECT_EQ(test.externalNested().field(), 15);

    const char *propertyName = "externalNested";

    ASSERT_TRUE(test.setProperty(propertyName, QVariant::fromValue<qtprotobufnamespace1::externaltests::NestedFieldMessage::NestedMessage*>(new qtprotobufnamespace1::externaltests::NestedFieldMessage::NestedMessage{55})));
    ASSERT_TRUE(*(test.property(propertyName).value<qtprotobufnamespace1::externaltests::NestedFieldMessage::NestedMessage*>()) == qtprotobufnamespace1::externaltests::NestedFieldMessage::NestedMessage{55});
    ASSERT_TRUE(test.externalNested() == qtprotobufnamespace1::externaltests::NestedFieldMessage::NestedMessage{55});
}

}
}
