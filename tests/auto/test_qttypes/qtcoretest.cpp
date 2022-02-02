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

#include <qtprotobuftestscommon.h>

#include "qttypes.qpb.h"

class QtTypesQtCoreTest : public ::testing::Test
{
public:
    // see simpletest.proto for property names and their field indices
    QtTypesQtCoreTest() {
    }

    static void SetUpTestCase() {
        qRegisterProtobufTypes();
        qRegisterProtobufQtTypes();
        serializer.reset(new QProtobufSerializer);
    }

    static std::unique_ptr<QProtobufSerializer> serializer;
};

std::unique_ptr<QProtobufSerializer> QtTypesQtCoreTest::serializer;

TEST_F(QtTypesQtCoreTest, QUrlTest)
{
    qProtobufAssertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QUrlMessage, QUrl>(1, "QUrl", "testField");

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
    qProtobufAssertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QCharMessage, QChar>(1, "QChar", "testField");

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
    qProtobufAssertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QUuidMessage, QUuid>(1, "QUuid", "testField");

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
    qProtobufAssertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QTimeMessage, QTime>(1, "QTime", "testField");

    qtprotobufnamespace::qttypes::tests::QTimeMessage msg;
    msg.setTestField(QTime(5, 30, 48, 123));

    QByteArray result = msg.serialize(serializer.get());
    msg.setTestField(QTime(7, 30, 18, 321));
    EXPECT_TRUE(QByteArray::fromHex("0a0508f6eef612") == result);

    msg.setTestField({});
    msg.deserialize(serializer.get(), QByteArray::fromHex("0a0508a291e219"));
    EXPECT_EQ(msg.testField().hour(), 7);
    EXPECT_EQ(msg.testField().minute(), 30);
    EXPECT_EQ(msg.testField().second(), 18);
    EXPECT_EQ(msg.testField().msec(), 321);
}

TEST_F(QtTypesQtCoreTest, QDateTest)
{
    qProtobufAssertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QDateMessage, QDate>(1, "QDate", "testField");
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
    qProtobufAssertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QDateTimeMessage, QDateTime>(1, "QDateTime", "testField");
    qtprotobufnamespace::qttypes::tests::QDateTimeMessage msg;
    msg.setTestField({QDate(1897, 3, 14), QTime(7, 30, 18, 321)});

    QByteArray result = msg.serialize(serializer.get());
    EXPECT_TRUE(QByteArray::fromHex("0a10120508a291e2190a07181c100608d21d") == result
                || QByteArray::fromHex("0a100a0708d21d1006181c120508a291e219") == result);

    msg.setTestField({});
    msg.deserialize(serializer.get(), QByteArray::fromHex("0a100a0708d21d1006181c120508a291e219"));
    EXPECT_TRUE(msg.testField() == QDateTime(QDate(1897, 3, 14), QTime(7, 30, 18, 321)));

    msg.setTestField({});
    msg.deserialize(serializer.get(), QByteArray::fromHex("0a100a0708801d100c1814120508a291e219"));
    EXPECT_TRUE(msg.testField() == QDateTime(QDate(1856, 6, 10), QTime(7, 30, 18, 321)));
}

TEST_F(QtTypesQtCoreTest, QSizeTest)
{
    qProtobufAssertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QSizeMessage, QSize>(1, "QSize", "testField");
    qtprotobufnamespace::qttypes::tests::QSizeMessage msg;
    msg.setTestField({1024, 768});

    QByteArray result = msg.serialize(serializer.get());
    EXPECT_TRUE(QByteArray::fromHex("0a0610800c088010") == result
                || QByteArray::fromHex("0a0608801010800c") == result);

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
    qProtobufAssertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QSizeFMessage, QSizeF>(1, "QSizeF", "testField");
    qtprotobufnamespace::qttypes::tests::QSizeFMessage msg;
    msg.setTestField({1024.0, 768.0});

    QByteArray result = msg.serialize(serializer.get());
    EXPECT_TRUE(QByteArray::fromHex("0a12110000000000008840090000000000009040") == result
                || QByteArray::fromHex("0a12090000000000009040110000000000008840") == result);

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
    qProtobufAssertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QPointMessage, QPoint>(1, "QPoint", "testField");
    qtprotobufnamespace::qttypes::tests::QPointMessage msg;
    msg.setTestField({1024, 768});

    QByteArray result = msg.serialize(serializer.get());
    EXPECT_TRUE(QByteArray::fromHex("0a0610800c088010") == result
                || QByteArray::fromHex("0a0608801010800c") == result);

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
    qProtobufAssertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QPointFMessage, QPointF>(1, "QPointF", "testField");
    qtprotobufnamespace::qttypes::tests::QPointFMessage msg;
    msg.setTestField({1024.0, 768.0});

    QByteArray result = msg.serialize(serializer.get());
    EXPECT_TRUE(QByteArray::fromHex("0a12110000000000008840090000000000009040") == result
                || QByteArray::fromHex("0a12090000000000009040110000000000008840") == result);

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
    qProtobufAssertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QRectMessage, QRect>(1, "QRect", "testField");
    qtprotobufnamespace::qttypes::tests::QRectMessage msg;
    msg.setTestField({QPoint(768, 768), QSize(500, 1212)});

    QByteArray result = msg.serialize(serializer.get());
    EXPECT_TRUE(QByteArray::fromHex("0a10120610f81208e8070a0610800c08800c") == result
                || QByteArray::fromHex("0a100a0608800c10800c120608e80710f812") == result);

    msg.setTestField({});

    msg.deserialize(serializer.get(), QByteArray::fromHex("0a100a0608800c10800c120608e80710f812"));
    EXPECT_EQ(msg.testField().x(), 768);
    EXPECT_EQ(msg.testField().y(), 768);
    EXPECT_EQ(msg.testField().width(), 500);
    EXPECT_EQ(msg.testField().height(), 1212);

    msg.setTestField({});

    msg.deserialize(serializer.get(), QByteArray::fromHex("0a0a0a00120608801010800c"));
    EXPECT_EQ(msg.testField().x(), 0);
    EXPECT_EQ(msg.testField().y(), 0);
    EXPECT_EQ(msg.testField().width(), 1024);
    EXPECT_EQ(msg.testField().height(), 768);
}

TEST_F(QtTypesQtCoreTest, QRectFTest)
{
    qProtobufAssertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QRectFMessage, QRectF>(1, "QRectF", "testField");
    qtprotobufnamespace::qttypes::tests::QRectFMessage msg;
    msg.setTestField({QPointF(768.0, 768.0), QPointF(1024.0, 1024.0)});

    QByteArray result = msg.serialize(serializer.get());
    std::cerr << result.toHex().toStdString() << std::endl;
    EXPECT_TRUE(QByteArray::fromHex("0a2812121100000000000070400900000000000070400a12110000000000008840090000000000008840") == result
                || QByteArray::fromHex("0a280a120900000000000088401100000000000088401212090000000000007040110000000000007040") == result);

    msg.setTestField({});

    msg.deserialize(serializer.get(), QByteArray::fromHex("0a280a120900000000000088401100000000000088401212090000000000007040110000000000007040"));
    EXPECT_EQ(msg.testField().x(), 768.0);
    EXPECT_EQ(msg.testField().y(), 768.0);
    EXPECT_EQ(msg.testField().width(), 256.0);
    EXPECT_EQ(msg.testField().height(), 256.0);

    msg.setTestField({});

    msg.deserialize(serializer.get(), QByteArray::fromHex("0a160a001212090000000000009040110000000000008840"));
    EXPECT_EQ(msg.testField().x(), 0.0);
    EXPECT_EQ(msg.testField().y(), 0.0);
    EXPECT_EQ(msg.testField().width(), 1024.0);
    EXPECT_EQ(msg.testField().height(), 768.0);
}

TEST_F(QtTypesQtCoreTest, QPolygonTest)
{
    qProtobufAssertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QPolygonMessage, QPolygon>(1, "QPolygon", "testField");
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
    qProtobufAssertMessagePropertyRegistered<qtprotobufnamespace::qttypes::tests::QPolygonFMessage, QPolygonF>(1, "QPolygonF", "testField");
    qtprotobufnamespace::qttypes::tests::QPolygonFMessage msg;
    msg.setTestField(QPolygonF({QPointF(10, 0), QPointF(20, 20), QPointF(0, 20)}));

    QByteArray result = msg.serialize(serializer.get());
    EXPECT_TRUE(QByteArray::fromHex("0a3c0a121100000000000000000900000000000024400a121100000000000034400900000000000034400a12110000000000003440090000000000000000") == result
                || QByteArray::fromHex("0a2a0a090900000000000024400a120900000000000034401100000000000034400a09110000000000003440") == result
                || QByteArray::fromHex("0a3c0a120900000000000024401100000000000000000a120900000000000034401100000000000034400a12090000000000000000110000000000003440") == result);

    msg.setTestField({});
    msg.deserialize(serializer.get(), QByteArray::fromHex("0a2a0a090900000000000024400a120900000000000034401100000000000034400a09110000000000003440"));
    EXPECT_EQ(msg.testField().size(), 3);
    EXPECT_EQ(msg.testField()[0], QPointF(10, 0));
    EXPECT_EQ(msg.testField()[1], QPointF(20, 20));
    EXPECT_EQ(msg.testField()[2], QPointF(0, 20));
}
