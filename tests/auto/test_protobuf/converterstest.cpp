/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>
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

#include <QVariant>
#include <qtprotobuftypes.h>

class ConverterTest : public ::testing::Test
{
public:
    ConverterTest() = default;
    static void SetUpTestCase() {
        qRegisterProtobufTypes();
    }
};

TEST_F(ConverterTest, TestFromTypeConverters)
{
    QVariant testVariant;

    testVariant.setValue<uint32_t>(42);
    EXPECT_EQ(42, testVariant.value<QtProtobuf::fixed32>()._t);

    testVariant.setValue<uint64_t>(43);
    EXPECT_EQ(43, testVariant.value<QtProtobuf::fixed64>()._t);

    testVariant.setValue<int32_t>(44);
    EXPECT_EQ(44, testVariant.value<QtProtobuf::sfixed32>()._t);

    testVariant.setValue<int64_t>(45);
    EXPECT_EQ(45, testVariant.value<QtProtobuf::sfixed64>()._t);

    testVariant.setValue<int32_t>(46);
    EXPECT_EQ(46, testVariant.value<QtProtobuf::int32>()._t);

    testVariant.setValue<int64_t>(47);
    EXPECT_EQ(47, testVariant.value<QtProtobuf::int64>()._t);
}

TEST_F(ConverterTest, TestToTypeConverters)
{
    bool ok = false;
    QVariant testVariant;
    testVariant.setValue<QtProtobuf::fixed32>({42});
    EXPECT_EQ(42, testVariant.toUInt(&ok));
    EXPECT_TRUE(ok);

    ok = false;
    testVariant.setValue<QtProtobuf::fixed64>({43});
    EXPECT_EQ(43, testVariant.toULongLong(&ok));
    EXPECT_TRUE(ok);

    ok = false;
    testVariant.setValue<QtProtobuf::sfixed32>({44});
    EXPECT_EQ(44, testVariant.toInt(&ok));
    EXPECT_TRUE(ok);

    ok = false;
    testVariant.setValue<QtProtobuf::sfixed64>({45});
    EXPECT_EQ(45, testVariant.toLongLong(&ok));
    EXPECT_TRUE(ok);

    ok = false;
    testVariant.setValue<QtProtobuf::int32>({46});
    EXPECT_EQ(46, testVariant.toInt(&ok));
    EXPECT_TRUE(ok);

    ok = false;
    testVariant.setValue<QtProtobuf::int64>({47});
    EXPECT_EQ(47, testVariant.toLongLong(&ok));
    EXPECT_TRUE(ok);
}

