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

#include "deserializationtest.h"
#include "qtprotobuf.h"
#include "simplefixedint32message.h"
#include "simplefixedint64message.h"
#include "simplefloatmessage.h"
#include "simpledoublemessage.h"
#include "simpleintmessage.h"

using namespace qtprotobufnamespace::tests;
using namespace qtprotobuf::tests;
using namespace qtprotobuf;

DeserializationTest::DeserializationTest()
{
    QtProtobuf::init();
}

TEST_F(DeserializationTest, FixedInt32MessageDeserializeTest)
{
    SimpleFixedInt32Message test;
    test.deserialize(QByteArray::fromHex("0d0f000000"));
    ASSERT_EQ(test.testFieldFixedInt32(), 15);

    test.deserialize(QByteArray::fromHex("0d2c010000"));
    ASSERT_EQ(test.testFieldFixedInt32(), 300);

    test.deserialize(QByteArray::fromHex("0d09000100"));
    ASSERT_EQ(test.testFieldFixedInt32(), 65545);

    test.deserialize(QByteArray::fromHex("0d00000000"));
    ASSERT_EQ(test.testFieldFixedInt32(), 0);

    test.deserialize(QByteArray::fromHex("0d2c010000"));
    ASSERT_EQ(test.testFieldFixedInt32(), 300);

    test.deserialize(QByteArray::fromHex("0d00010000"));
    ASSERT_EQ(test.testFieldFixedInt32(), UINT8_MAX + 1);

    test.deserialize(QByteArray::fromHex("0d00000100"));
    ASSERT_EQ(test.testFieldFixedInt32(), UINT16_MAX + 1);

    test.deserialize(QByteArray::fromHex("0dff000000"));
    ASSERT_EQ(test.testFieldFixedInt32(), UINT8_MAX);

    test.deserialize(QByteArray::fromHex("0dffff0000"));
    ASSERT_EQ(test.testFieldFixedInt32(), UINT16_MAX);

    test.deserialize(QByteArray::fromHex("0dffffffff"));
    ASSERT_EQ(test.testFieldFixedInt32(), UINT32_MAX);
}

TEST_F(DeserializationTest, FixedInt64MessageDeserializeTest)
{
    SimpleFixedInt64Message test;
    test.deserialize(QByteArray::fromHex("090f00000000000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), 15);


    test.deserialize(QByteArray::fromHex("092c01000000000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), 300);

    test.deserialize(QByteArray::fromHex("090900010000000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), 65545);

    test.deserialize(QByteArray::fromHex("090000000000000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), 0);

    test.deserialize(QByteArray::fromHex("090001000000000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), UINT8_MAX + 1);

    test.deserialize(QByteArray::fromHex("090000010000000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), UINT16_MAX + 1);

    test.deserialize(QByteArray::fromHex("090000000001000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), (unsigned long long)(UINT32_MAX) + 1);

    test.deserialize(QByteArray::fromHex("09ff00000000000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), UINT8_MAX);

    test.deserialize(QByteArray::fromHex("09ffff000000000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), UINT16_MAX);

    test.deserialize(QByteArray::fromHex("09ffffffff00000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), UINT32_MAX);

    test.deserialize(QByteArray::fromHex("09ffffffffffffffff"));
    ASSERT_EQ(test.testFieldFixedInt64(), UINT64_MAX);
}


TEST_F(DeserializationTest, FloatMessageDeserializeTest)
{
    SimpleFloatMessage test;
    test.deserialize(QByteArray::fromHex("3dcdcccc3d"));
    ASSERT_FLOAT_EQ(test.testFieldFloat(), 0.1f);

    test.deserialize(QByteArray::fromHex("3d00008000"));
    ASSERT_FLOAT_EQ(test.testFieldFloat(), FLT_MIN);

    test.deserialize(QByteArray::fromHex("3dffff7f7f"));
    ASSERT_FLOAT_EQ(test.testFieldFloat(), FLT_MAX);

    test.deserialize(QByteArray::fromHex("3d666686c0"));
    ASSERT_FLOAT_EQ(test.testFieldFloat(), -4.2f);

    test.deserialize(QByteArray::fromHex("3d00000000"));
    ASSERT_FLOAT_EQ(test.testFieldFloat(), -0.0f);
}

TEST_F(DeserializationTest, DoubleMessageDeserializeTest)
{
    SimpleDoubleMessage test;
    test.deserialize(QByteArray::fromHex("419a9999999999b93f"));
    ASSERT_DOUBLE_EQ(0.1, test.testFieldDouble());

    test.deserialize(QByteArray::fromHex("410000000000001000"));
    ASSERT_DOUBLE_EQ(DBL_MIN, test.testFieldDouble());

    test.deserialize(QByteArray::fromHex("41ffffffffffffef7f"));
    ASSERT_DOUBLE_EQ(DBL_MAX, test.testFieldDouble());

    test.deserialize(QByteArray::fromHex("41cdcccccccccc10c0"));
    ASSERT_DOUBLE_EQ(-4.2, test.testFieldDouble());

    test.deserialize(QByteArray::fromHex("410000000000000000"));
    ASSERT_DOUBLE_EQ(0.0, test.testFieldDouble());
}

TEST_F(DeserializationTest, IntMessageDeserializeTest)
{
    SimpleIntMessage test;
    test.deserialize(QByteArray::fromHex("081e"));
    ASSERT_EQ(15, test.testFieldInt());

    test.deserialize(QByteArray::fromHex("08d804"));
    ASSERT_EQ(300, test.testFieldInt());

    test.deserialize(QByteArray::fromHex("08928008"));
    ASSERT_EQ(65545, test.testFieldInt());

    test.deserialize(QByteArray::fromHex("0800"));
    ASSERT_EQ(0, test.testFieldInt());

    test.deserialize(QByteArray::fromHex("088002"));
    ASSERT_EQ(INT8_MAX + 1, test.testFieldInt());

    test.deserialize(QByteArray::fromHex("08808004"));
    ASSERT_EQ(INT16_MAX + 1, test.testFieldInt());

    test.deserialize(QByteArray::fromHex("08fe01"));
    ASSERT_EQ(INT8_MAX, test.testFieldInt());

    test.deserialize(QByteArray::fromHex("08feff03"));
    ASSERT_EQ(INT16_MAX, test.testFieldInt());

    test.deserialize(QByteArray::fromHex("08feffffff0f"));
    ASSERT_EQ(INT32_MAX, test.testFieldInt());

    test.deserialize(QByteArray::fromHex("0801"));
    ASSERT_EQ(-1, test.testFieldInt());

    test.deserialize(QByteArray::fromHex("0801"));
    ASSERT_EQ(-1, test.testFieldInt());

    test.deserialize(QByteArray::fromHex("089b07"));
    ASSERT_EQ(-462, test.testFieldInt());

    test.deserialize(QByteArray::fromHex("08c1e107"));
    ASSERT_EQ(-63585, test.testFieldInt());

    test.deserialize(QByteArray::fromHex("08ff01"));
    ASSERT_EQ(INT8_MIN, test.testFieldInt());

    test.deserialize(QByteArray::fromHex("08ffff03"));
    ASSERT_EQ(INT16_MIN, test.testFieldInt());

    test.deserialize(QByteArray::fromHex("08ffffffff0f"));
    ASSERT_EQ(INT32_MIN, test.testFieldInt());

    test.deserialize(QByteArray::fromHex("088102"));
    ASSERT_EQ(INT8_MIN - 1, test.testFieldInt());

    test.deserialize(QByteArray::fromHex("08818004"));
    ASSERT_EQ(INT16_MIN - 1, test.testFieldInt());
}

