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

namespace QtProtobuf {
namespace tests {

class CoverterTest : public ::testing::Test
{
public:
    CoverterTest() = default;
    static void SetUpTestCase() {
        QtProtobuf::qRegisterProtobufTypes();
    }
};

TEST_F(CoverterTest, TestFromTypeConverters)
{
    QVariant testVariant;

    testVariant.setValue<uint32_t>(42);
    ASSERT_EQ(42, testVariant.value<fixed32>()._t);

    testVariant.setValue<uint64_t>(43);
    ASSERT_EQ(43, testVariant.value<fixed64>()._t);

    testVariant.setValue<int32_t>(44);
    ASSERT_EQ(44, testVariant.value<sfixed32>()._t);

    testVariant.setValue<int64_t>(45);
    ASSERT_EQ(45, testVariant.value<sfixed64>()._t);

    testVariant.setValue<int32_t>(46);
    ASSERT_EQ(46, testVariant.value<int32>()._t);

    testVariant.setValue<int64_t>(47);
    ASSERT_EQ(47, testVariant.value<int64>()._t);
}

TEST_F(CoverterTest, TestToTypeConverters)
{
    bool ok = false;
    QVariant testVariant;
    testVariant.setValue<fixed32>({42});
    ASSERT_EQ(42, testVariant.toUInt(&ok));
    ASSERT_TRUE(ok);

    ok = false;
    testVariant.setValue<fixed64>({43});
    ASSERT_EQ(43, testVariant.toULongLong(&ok));
    ASSERT_TRUE(ok);

    ok = false;
    testVariant.setValue<sfixed32>({44});
    ASSERT_EQ(44, testVariant.toInt(&ok));
    ASSERT_TRUE(ok);

    ok = false;
    testVariant.setValue<sfixed64>({45});
    ASSERT_EQ(45, testVariant.toLongLong(&ok));
    ASSERT_TRUE(ok);

    ok = false;
    testVariant.setValue<int32>({46});
    ASSERT_EQ(46, testVariant.toInt(&ok));
    ASSERT_TRUE(ok);

    ok = false;
    testVariant.setValue<int64>({47});
    ASSERT_EQ(47, testVariant.toLongLong(&ok));
    ASSERT_TRUE(ok);
}

}
}
