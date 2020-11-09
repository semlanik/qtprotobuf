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

#define private public
#define protected public
#include "simpletest.qpb.h"
#include "sequencetest.qpb.h"
#include "externalpackagetest.qpb.h"
#include "globalenums.qpb.h"
#include "globalenumssamenamespace.qpb.h"
#include "nestedmessages.qpb.h"
#undef private
#undef protected

#include <gtest/gtest.h>
#include "../testscommon.h"

using namespace qtprotobufnamespace::tests;
using namespace qtprotobufnamespace::tests::nested;
using namespace qtprotobufnamespace::tests::sequence;

namespace QtProtobuf {
namespace tests {

class InternalsTest : public ::testing::Test
{
public:
    InternalsTest() = default;
};

TEST_F(InternalsTest, NullPointerMessageTest)
{
    ComplexMessage msg(0, {QString("not null")});
    msg.setTestComplexField_p(nullptr);
    ASSERT_TRUE(msg.testComplexField().testFieldString().isEmpty());
    ASSERT_TRUE(msg.testComplexField_p() != nullptr);
}

TEST_F(InternalsTest, NullPointerGetterMessageTest)
{
    ComplexMessage msg;
    ASSERT_TRUE(msg.testComplexField_p() != nullptr);
    msg.setTestComplexField_p(nullptr);
    ASSERT_TRUE(msg.testComplexField().testFieldString().isEmpty());
    ASSERT_TRUE(msg.testComplexField_p() != nullptr);
}

}
}
