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

#include <QVariantList>
#include <QMetaProperty>
#include <QSignalSpy>
#include <QProtobufSerializer>
#include <QProtobufJsonSerializer>
#include <QDateTime>

#include <stdio.h>
#include <iostream>
#include <gtest/gtest.h>

#include "../testscommon.h"

#include "qttypes.qpb.h"

namespace QtProtobuf {
namespace tests {

class QtTypesQtGuiTest : public ::testing::Test
{
public:
    // see simpletest.proto for property names and their field indices
    QtTypesQtGuiTest() {
    }

    static void SetUpTestCase() {
        QtProtobuf::qRegisterProtobufTypes();
        QtProtobuf::qRegisterProtobufQtTypes();
        serializer.reset(new QProtobufSerializer);
    }

    static std::unique_ptr<QProtobufSerializer> serializer;
};

std::unique_ptr<QProtobufSerializer> QtTypesQtGuiTest::serializer;

TEST_F(QtTypesQtGuiTest, QColorTest)
{
    assertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QColorMessage, QColor>(1, "QColor", "testField");

    qtprotobufnamespace::qttypes::tests::QColorMessage msg;
    msg.setTestField(QColor("red"));

    auto result = msg.serialize(serializer.get());
    EXPECT_TRUE(QByteArray::fromHex("0a06088080fcff0f") == result);

    msg.setTestField({});
    msg.deserialize(serializer.get(), QByteArray::fromHex("0a0608808082f80f"));

    EXPECT_EQ(QColor("green"), msg.testField());
}

TEST_F(QtTypesQtGuiTest, QMatrix4x4Test)
{
    assertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QMatrix4x4Message, QMatrix4x4>(1, "QMatrix4x4", "testField");

    qtprotobufnamespace::qttypes::tests::QMatrix4x4Message msg;
    msg.setTestField({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});
    auto result = msg.serialize(serializer.get());

    EXPECT_TRUE(QByteArray::fromHex("0a518501000070417d0000604175000050416d0000404165000030415d0000204155000010414d00000041450000e0403d0000c040350000a0402d0000804025000040401d00000040150000803f0d00000000") == result
                || QByteArray::fromHex("0a4c150000803f1d0000004025000040402d00008040350000a0403d0000c040450000e0404d0000004155000010415d0000204165000030416d0000404175000050417d00006041850100007041") == result
                || QByteArray::fromHex("0a514d000000410d00000000150000803f1d0000004025000040402d00008040350000a0403d0000c040450000e04055000010415d0000204165000030416d0000404175000050417d00006041850100007041") == result);

    msg.setTestField({});
    msg.deserialize(serializer.get(), QByteArray::fromHex("0a4c150000803f1d0000004025000040402d00008040350000a0403d0000c040450000e0404d0000004155000010415d0000204165000030416d0000404175000050417d00006041850100007041"));

    EXPECT_EQ(QMatrix4x4(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15), msg.testField());
}

TEST_F(QtTypesQtGuiTest, QVector2DTest)
{
    assertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QVector2DMessage, QVector2D>(1, "QVector2D", "testField");

    qtprotobufnamespace::qttypes::tests::QVector2DMessage msg;
    msg.setTestField(QVector2D(42, 24));
    auto result = msg.serialize(serializer.get());

    EXPECT_TRUE(QByteArray::fromHex("0a0a150000c0410d00002842") == result
                || QByteArray::fromHex("0a0a0d00002842150000c041") == result);

    msg.setTestField({});
    msg.deserialize(serializer.get(), QByteArray::fromHex("0a0a0d0000c0411500002842"));

    EXPECT_EQ(QVector2D(24, 42), msg.testField());
}

TEST_F(QtTypesQtGuiTest, QVector3DTest)
{
    assertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QVector3DMessage, QVector3D>(1, "QVector3D", "testField");

    qtprotobufnamespace::qttypes::tests::QVector3DMessage msg;
    msg.setTestField(QVector3D(42, 24, 11));
    auto result = msg.serialize(serializer.get());

    EXPECT_TRUE(QByteArray::fromHex("0a0f1d00003041150000c0410d00002842") == result
                || QByteArray::fromHex("0a0f0d00002842150000c0411d00003041") == result);

    msg.setTestField({});
    msg.deserialize(serializer.get(), QByteArray::fromHex("0a0f0d0000c04115000030411d00002842"));

    EXPECT_EQ(QVector3D(24, 11, 42), msg.testField());
}

TEST_F(QtTypesQtGuiTest, QVector4DTest)
{
    assertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QVector4DMessage, QVector4D>(1, "QVector4D", "testField");

    qtprotobufnamespace::qttypes::tests::QVector4DMessage msg;
    msg.setTestField({24, 11, 42, 0});
    auto result = msg.serialize(serializer.get());

    EXPECT_TRUE(QByteArray::fromHex("0a1425000000001d0000284215000030410d0000c041") == result
                || QByteArray::fromHex("0a0f0d0000c04115000030411d00002842") == result
                || QByteArray::fromHex("0a140d0000c04115000030411d000028422500000000") == result);

    msg.setTestField({});
    msg.deserialize(serializer.get(), QByteArray::fromHex("0a0f0d0000c04115000030411d00002842"));

    EXPECT_EQ(QVector4D(24, 11, 42, 0), msg.testField());
}

TEST_F(QtTypesQtGuiTest, QTransformTest)
{
    assertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QTransformMessage, QTransform>(1, "QTransform", "testField");

    qtprotobufnamespace::qttypes::tests::QTransformMessage msg;
    msg.setTestField(QTransform(0, 1, 2, 3, 4, 5, 6, 7, 8));
    auto result = msg.serialize(serializer.get());

    EXPECT_TRUE(QByteArray::fromHex("0a51490000000000002040410000000000001c4039000000000000184031000000000000144029000000000000104021000000000000084019000000000000004011000000000000f03f090000000000000000") == result
                || QByteArray::fromHex("0a4811000000000000f03f190000000000000040210000000000000840290000000000001040310000000000001440390000000000001840410000000000001c40490000000000002040") == result
                || QByteArray::fromHex("0a5149000000000000204009000000000000000011000000000000f03f190000000000000040210000000000000840290000000000001040310000000000001440390000000000001840410000000000001c40") == result);

    msg.setTestField({});
    msg.deserialize(serializer.get(), QByteArray::fromHex("0a48090000000000002040110000000000001c4019000000000000184021000000000000144029000000000000104031000000000000084039000000000000004041000000000000f03f"));
    EXPECT_EQ(QTransform(8, 7, 6, 5, 4, 3, 2, 1, 0), msg.testField());
}

TEST_F(QtTypesQtGuiTest, QQuaternionTest)
{
    assertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QQuaternionMessage, QQuaternion>(1, "QQuaternion", "testField");

    qtprotobufnamespace::qttypes::tests::QQuaternionMessage msg;
    msg.setTestField(QQuaternion(14, 10, 24, 22));

    auto result = msg.serialize(serializer.get());
    EXPECT_TRUE(QByteArray::fromHex("0a16120f1d0000b041150000c0410d000020410d00006041") ==
               result || QByteArray::fromHex("0a160d00006041120f0d00002041150000c0411d0000b041") == result);

    msg.setTestField({});
    msg.deserialize(serializer.get(), QByteArray::fromHex("0a160d00006041120f0d00002041150000c0411d0000b041"));

    EXPECT_EQ(QQuaternion(14, 10, 24, 22), msg.testField());
}

TEST_F(QtTypesQtGuiTest, QImageTest)
{
    assertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QImageMessage, QImage>(1, "QImage", "testField");

    qtprotobufnamespace::qttypes::tests::QImageMessage msg;
    QImage initialImage("./testimage.png");
    msg.setTestField(QImage("./testimage.png"));
    ASSERT_FALSE(msg.testField().isNull());

    auto result = msg.serialize(serializer.get());
    msg.setTestField({});
    msg.deserialize(serializer.get(), result);
    EXPECT_EQ(initialImage, msg.testField());
}
}
}
