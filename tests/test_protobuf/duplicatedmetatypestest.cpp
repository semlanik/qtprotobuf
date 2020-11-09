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

#include "duplicated_metatypes.qpb.h"
#include "../testscommon.h"

namespace QtProtobuf {
namespace tests {

class DuplicatedMetatypesTest : public ::testing::Test
{
public:
    DuplicatedMetatypesTest() = default;
    void SetUp() override;
    static void SetUpTestCase();
};

void DuplicatedMetatypesTest::SetUpTestCase()
{
    //Register all types
    QtProtobuf::qRegisterProtobufTypes();
}

void DuplicatedMetatypesTest::SetUp()
{
}


TEST_F(DuplicatedMetatypesTest, SimpleTest) {
    assertMessagePropertyRegistered<qtprotobufnamespace::duplicated_metatypes::Message1, qtprotobufnamespace::duplicated_metatypes::Message1::OptsEntry>(1, "qtprotobufnamespace::duplicated_metatypes::Message1::OptsEntry", "opts");
    assertMessagePropertyRegistered<qtprotobufnamespace::duplicated_metatypes::Message2, qtprotobufnamespace::duplicated_metatypes::Message2::OptsEntry>(1, "qtprotobufnamespace::duplicated_metatypes::Message2::OptsEntry", "opts");
    assertMessagePropertyRegistered<qtprotobufnamespace::duplicated_metatypes::Message3, qtprotobufnamespace::duplicated_metatypes::Message3::OptsEntry>(1, "qtprotobufnamespace::duplicated_metatypes::Message3::OptsEntry", "opts");
    assertMessagePropertyRegistered<qtprotobufnamespace::duplicated_metatypes::Message4, qtprotobufnamespace::duplicated_metatypes::Message4::OptsEntry>(1, "qtprotobufnamespace::duplicated_metatypes::Message4::OptsEntry", "opts");
    assertMessagePropertyRegistered<qtprotobufnamespace::duplicated_metatypes::Message5, qtprotobufnamespace::duplicated_metatypes::Message5::OptsEntry>(1, "qtprotobufnamespace::duplicated_metatypes::Message5::OptsEntry", "opts");
    assertMessagePropertyRegistered<qtprotobufnamespace::duplicated_metatypes::Message6, qtprotobufnamespace::duplicated_metatypes::Message6::OptsEntry>(1, "qtprotobufnamespace::duplicated_metatypes::Message6::OptsEntry", "opts");
    assertMessagePropertyRegistered<qtprotobufnamespace::duplicated_metatypes::Message7, qtprotobufnamespace::duplicated_metatypes::Message7::OptsEntry>(1, "qtprotobufnamespace::duplicated_metatypes::Message7::OptsEntry", "opts");
    assertMessagePropertyRegistered<qtprotobufnamespace::duplicated_metatypes::Message8, qtprotobufnamespace::duplicated_metatypes::Message8::OptsEntry>(1, "qtprotobufnamespace::duplicated_metatypes::Message8::OptsEntry", "opts");
}

}
}
