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

class QtTypesQtCoreTest : public ::testing::Test
{
public:
    // see simpletest.proto for property names and their field indices
    QtTypesQtCoreTest() {
    }

    static void SetUpTestCase() {
        QtProtobuf::qRegisterProtobufTypes();
        QtProtobuf::qRegisterProtobufQtTypes();
        serializer.reset(new QProtobufSerializer);
    }

    static std::unique_ptr<QProtobufSerializer> serializer;
};

std::unique_ptr<QProtobufSerializer> QtTypesQtCoreTest::serializer;

TEST_F(QtTypesQtCoreTest, QUrlTest)
{
    assertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QUrlMessage, QUrl>(1, "QUrl", "testField");

    qtprotobufnamespace::qttypes::tests::QUrlMessage msg;
    msg.setTestField(QUrl("https://github.com/semlanik/qtprotobuf"));
    EXPECT_TRUE(QByteArray::fromHex("0a280a2668747470733a2f2f6769746875622e636f6d2f73656d6c616e696b2f717470726f746f627566") ==
                msg.serialize(serializer.get()));

    msg.setTestField({});
    msg.deserialize(serializer.get(), QByteArray::fromHex("0a1d0a1b68747470733a2f2f6769746875622e636f6d2f73656d6c616e696b"));

    EXPECT_STREQ("https://github.com/semlanik", msg.testField().url().toStdString().c_str());
}

TEST_F(QtTypesQtCoreTest, QCharTest)
{
    assertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QCharMessage, QChar>(1, "QChar", "testField");

    qtprotobufnamespace::qttypes::tests::QCharMessage msg;
    msg.setTestField(QChar('q'));
    EXPECT_TRUE(QByteArray::fromHex("0a040a020071") ==
                msg.serialize(serializer.get()));

    msg.setTestField({});
    msg.deserialize(serializer.get(), QByteArray::fromHex("0a040a0220ac"));

    EXPECT_TRUE(QChar(8364) == msg.testField());
}


TEST_F(QtTypesQtCoreTest, QUuidTest)
{
    assertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QUuidMessage, QUuid>(1, "QUuid", "testField");

    qtprotobufnamespace::qttypes::tests::QUuidMessage msg;
    msg.setTestField(QUuid("{4bcbcdc3-c5b3-4d34-97fe-af78c825cc7d}"));

    EXPECT_TRUE(QByteArray::fromHex("0a280a267b34626362636463332d633562332d346433342d393766652d6166373863383235636337647d") ==
                msg.serialize(serializer.get()));

    msg.setTestField({});
    msg.deserialize(serializer.get(), QByteArray::fromHex("0a280a267b34626362636463332d633562332d346433342d393766652d6166373863383235636337647d"));

    EXPECT_TRUE(QUuid("{4bcbcdc3-c5b3-4d34-97fe-af78c825cc7d}") == msg.testField());
}

TEST_F(QtTypesQtCoreTest, QTimeTest)
{
    assertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QTimeMessage, QTime>(1, "QTime", "testField");

    qtprotobufnamespace::qttypes::tests::QTimeMessage msg;
    msg.setTestField(QTime(5, 30, 48, 123));
    QByteArray result = msg.serialize(serializer.get());
    EXPECT_TRUE(QByteArray::fromHex("0a09080a103c186020f601") ==
                result || QByteArray::fromHex("0a0920f6011860103c080a") == result);

    msg.setTestField({});
    msg.deserialize(serializer.get(), QByteArray::fromHex("0a09080e103c1824208205"));
    EXPECT_EQ(msg.testField().hour(), 7);
    EXPECT_EQ(msg.testField().minute(), 30);
    EXPECT_EQ(msg.testField().second(), 18);
    EXPECT_EQ(msg.testField().msec(), 321);
}

TEST_F(QtTypesQtCoreTest, QDateTest)
{
    assertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QDateMessage, QDate>(1, "QDate", "testField");
    qtprotobufnamespace::qttypes::tests::QDateMessage msg;
    msg.setTestField(QDate(1856, 6, 10));
    QByteArray result = msg.serialize(serializer.get());
    EXPECT_TRUE(QByteArray::fromHex("0a0708801d100c1814") ==
                result || QByteArray::fromHex("0a071814100c08801d") == result);

    msg.setTestField({});
    msg.deserialize(serializer.get(), QByteArray::fromHex("0a0708d21d1006181c"));
    EXPECT_EQ(msg.testField().year(), 1897);
    EXPECT_EQ(msg.testField().month(), 3);
    EXPECT_EQ(msg.testField().day(), 14);
}

TEST_F(QtTypesQtCoreTest, QDateTimeTest)
{
    assertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QDateTimeMessage, QDateTime>(1, "QDateTime", "testField");
    qtprotobufnamespace::qttypes::tests::QDateTimeMessage msg;
    msg.setTestField({QDate(1897, 3, 14), QTime(7, 30, 12, 321)});
    QByteArray result = msg.serialize(serializer.get());

    EXPECT_TRUE(QByteArray::fromHex("0a1412092082051818103c080e0a07181c100608d21d") ==
                result || QByteArray::fromHex("0a140a0708d21d1006181c1209080e103c1818208205") == result);

    msg.setTestField({});
    msg.deserialize(serializer.get(), QByteArray::fromHex("0a140a0708d21d1006181c1209080e103c1818208205"));
    EXPECT_TRUE(msg.testField() == QDateTime(QDate(1897, 3, 14), QTime(7, 30, 12, 321)));

    msg.deserialize(serializer.get(), QByteArray::fromHex("0a140a0708801d100c18141209080a103c186020f601"));
    EXPECT_TRUE(msg.testField() == QDateTime(QDate(1856, 6, 10), QTime(5, 30, 48, 123)));
}

TEST_F(QtTypesQtCoreTest, QSizeTest)
{
    assertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QSizeMessage, QSize>(1, "QSize", "testField");
    qtprotobufnamespace::qttypes::tests::QSizeMessage msg;
    msg.setTestField({1024, 768});
    QByteArray result = msg.serialize(serializer.get());

    EXPECT_TRUE(QByteArray::fromHex("0a0610800c088010") ==
                result || QByteArray::fromHex("0a0608801010800c") == result);

    msg.setTestField({});

    msg.deserialize(serializer.get(), QByteArray::fromHex("0a0608801010800c"));
    EXPECT_EQ(msg.testField().width(), 1024);
    EXPECT_EQ(msg.testField().height(), 768);

    msg.deserialize(serializer.get(), QByteArray::fromHex("0a0608800c108010"));
    EXPECT_EQ(msg.testField().width(), 768);
    EXPECT_EQ(msg.testField().height(), 1024);
}

TEST_F(QtTypesQtCoreTest, QSizeFTest)
{
    assertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QSizeFMessage, QSizeF>(1, "QSizeF", "testField");
    qtprotobufnamespace::qttypes::tests::QSizeFMessage msg;
    msg.setTestField({1024.0, 768.0});
    QByteArray result = msg.serialize(serializer.get());

    EXPECT_TRUE(QByteArray::fromHex("0a12110000000000008840090000000000009040") ==
                result || QByteArray::fromHex("0a12090000000000009040110000000000008840") == result);

    msg.setTestField({});

    msg.deserialize(serializer.get(), QByteArray::fromHex("0a12090000000000009040110000000000008840"));
    EXPECT_EQ(msg.testField().width(), 1024.0);
    EXPECT_EQ(msg.testField().height(), 768.0);

    msg.deserialize(serializer.get(), QByteArray::fromHex("0a12090000000000008840110000000000009040"));
    EXPECT_EQ(msg.testField().width(), 768.0);
    EXPECT_EQ(msg.testField().height(), 1024.0);
}

TEST_F(QtTypesQtCoreTest, QPointTest)
{
    assertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QPointMessage, QPoint>(1, "QPoint", "testField");
    qtprotobufnamespace::qttypes::tests::QPointMessage msg;
    msg.setTestField({1024, 768});
    QByteArray result = msg.serialize(serializer.get());

    EXPECT_TRUE(QByteArray::fromHex("0a0610800c088010") ==
                result || QByteArray::fromHex("0a0608801010800c") == result);

    msg.setTestField({});

    msg.deserialize(serializer.get(), QByteArray::fromHex("0a0608801010800c"));
    EXPECT_EQ(msg.testField().x(), 1024);
    EXPECT_EQ(msg.testField().y(), 768);

    msg.deserialize(serializer.get(), QByteArray::fromHex("0a0608800c108010"));
    EXPECT_EQ(msg.testField().x(), 768);
    EXPECT_EQ(msg.testField().y(), 1024);
}

TEST_F(QtTypesQtCoreTest, QPointFTest)
{
    assertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QPointFMessage, QPointF>(1, "QPointF", "testField");
    qtprotobufnamespace::qttypes::tests::QPointFMessage msg;
    msg.setTestField({1024.0, 768.0});
    QByteArray result = msg.serialize(serializer.get());

    EXPECT_TRUE(QByteArray::fromHex("0a12110000000000008840090000000000009040") ==
                result || QByteArray::fromHex("0a12090000000000009040110000000000008840") == result);

    msg.setTestField({});

    msg.deserialize(serializer.get(), QByteArray::fromHex("0a12090000000000009040110000000000008840"));
    EXPECT_EQ(msg.testField().x(), 1024.0);
    EXPECT_EQ(msg.testField().y(), 768.0);

    msg.deserialize(serializer.get(), QByteArray::fromHex("0a12090000000000008840110000000000009040"));
    EXPECT_EQ(msg.testField().x(), 768.0);
    EXPECT_EQ(msg.testField().y(), 1024.0);
}

TEST_F(QtTypesQtCoreTest, QRectTest)
{
    assertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QRectMessage, QRect>(1, "QRect", "testField");
    qtprotobufnamespace::qttypes::tests::QRectMessage msg;
    msg.setTestField({QPoint(768, 768), QPoint(1024, 1024)});
    QByteArray result = msg.serialize(serializer.get());

    EXPECT_TRUE(QByteArray::fromHex("0a1012061080100880100a0610800c08800c") ==
                result || QByteArray::fromHex("0a100a0608800c10800c1206088010108010") == result);
    msg.setTestField({});

    msg.deserialize(serializer.get(), QByteArray::fromHex("0a100a0608800c10800c1206088010108010"));
    EXPECT_EQ(msg.testField().x(), 768);
    EXPECT_EQ(msg.testField().y(), 768);
    EXPECT_EQ(msg.testField().width(), 257); //WTF Qt, siriously why?
    EXPECT_EQ(msg.testField().height(), 257); //WTF Qt, siriously why?

    msg.setTestField({QPoint(0, 0), QPoint(1024, 768)});

    msg.deserialize(serializer.get(), QByteArray::fromHex("0a0a0a00120608801010800c"));
    EXPECT_EQ(msg.testField().x(), 0);
    EXPECT_EQ(msg.testField().y(), 0);
    EXPECT_EQ(msg.testField().width(), 1025); //WTF Qt, siriously why?
    EXPECT_EQ(msg.testField().height(), 769); //WTF Qt, siriously why?
}

TEST_F(QtTypesQtCoreTest, QRectFTest)
{
    assertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QRectFMessage, QRectF>(1, "QRectF", "testField");
    qtprotobufnamespace::qttypes::tests::QRectFMessage msg;
    msg.setTestField({QPointF(768.0, 768.0), QPointF(1024.0, 1024.0)});
    QByteArray result = msg.serialize(serializer.get());

    EXPECT_TRUE(QByteArray::fromHex("0a2812121100000000000090400900000000000090400a12110000000000008840090000000000008840") ==
                result || QByteArray::fromHex("0a280a120900000000000088401100000000000088401212090000000000009040110000000000009040") == result);
    msg.setTestField({});

    msg.deserialize(serializer.get(), QByteArray::fromHex("0a280a120900000000000088401100000000000088401212090000000000009040110000000000009040"));
    EXPECT_EQ(msg.testField().x(), 768.0);
    EXPECT_EQ(msg.testField().y(), 768.0);
    EXPECT_EQ(msg.testField().width(), 256.0);
    EXPECT_EQ(msg.testField().height(), 256.0);

    msg.setTestField({QPointF(0, 0), QPointF(1024.0, 768.0)});

    msg.deserialize(serializer.get(), QByteArray::fromHex("0a160a001212090000000000009040110000000000008840"));
    EXPECT_EQ(msg.testField().x(), 0.0);
    EXPECT_EQ(msg.testField().y(), 0.0);
    EXPECT_EQ(msg.testField().width(), 1024.0);
    EXPECT_EQ(msg.testField().height(), 768.0);
}

TEST_F(QtTypesQtCoreTest, QPolygonTest)
{
    assertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QPolygonMessage, QPolygon>(1, "QPolygon", "testField");
    qtprotobufnamespace::qttypes::tests::QPolygonMessage msg;
    msg.setTestField(QPolygon({QPoint(10, 0), QPoint(20, 20), QPoint(0, 20)}));
    QByteArray result = msg.serialize(serializer.get());
    EXPECT_TRUE(QByteArray::fromHex("0a0e0a0208140a04102808280a021028") ==
                result || QByteArray::fromHex("0a0e0a0208140a04082810280a021028") == result);

    msg.setTestField({});
    msg.deserialize(serializer.get(), QByteArray::fromHex("0a0e0a0208140a04082810280a021028"));

    EXPECT_EQ(msg.testField().size(), 3);
    EXPECT_EQ(msg.testField()[0], QPoint(10, 0));
    EXPECT_EQ(msg.testField()[1], QPoint(20, 20));
    EXPECT_EQ(msg.testField()[2], QPoint(0, 20));
}

TEST_F(QtTypesQtCoreTest, QPolygonFTest)
{
    assertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QPolygonFMessage, QPolygonF>(1, "QPolygonF", "testField");
    qtprotobufnamespace::qttypes::tests::QPolygonFMessage msg;
    msg.setTestField(QPolygonF({QPointF(10, 0), QPointF(20, 20), QPointF(0, 20)}));
    QByteArray result = msg.serialize(serializer.get());
    EXPECT_TRUE(QByteArray::fromHex("0a3c0a121100000000000000000900000000000024400a121100000000000034400900000000000034400a12110000000000003440090000000000000000") ==
                result || QByteArray::fromHex("0a3c0a090900000000000024400a120900000000000034401100000000000034400a09110000000000003440") == result);

    msg.setTestField({});
    msg.deserialize(serializer.get(), QByteArray::fromHex("0a2a0a090900000000000024400a120900000000000034401100000000000034400a09110000000000003440"));

    EXPECT_EQ(msg.testField().size(), 3);
    EXPECT_EQ(msg.testField()[0], QPointF(10, 0));
    EXPECT_EQ(msg.testField()[1], QPointF(20, 20));
    EXPECT_EQ(msg.testField()[2], QPointF(0, 20));
}

}
}
