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

#include "serializationtest.h"

#include "simplefixed32complexmessagemapmessage.h"
#include "simplesfixed32complexmessagemapmessage.h"
#include "simpleint32complexmessagemapmessage.h"
#include "simplesint32complexmessagemapmessage.h"
#include "simpleuint32complexmessagemapmessage.h"

#include "simplefixed64complexmessagemapmessage.h"
#include "simplesfixed64complexmessagemapmessage.h"
#include "simpleint64complexmessagemapmessage.h"
#include "simplesint64complexmessagemapmessage.h"
#include "simpleuint64complexmessagemapmessage.h"

#include "simplestringcomplexmessagemapmessage.h"

using namespace qtprotobufnamespace::tests;
using namespace QtProtobuf::tests;
using namespace QtProtobuf;

//Complex map
TEST_F(SerializationTest, SimpleFixed32ComplexMapSerializeTest)
{
    SimpleFixed32ComplexMessageMapMessage test;
    test.setMapField({{10, QSharedPointer<ComplexMessage>(new ComplexMessage{16, {"ten sixteen"}})}, {42, QSharedPointer<ComplexMessage>(new ComplexMessage{10, {"fourty two ten sixteen"}})}, {65555, QSharedPointer<ComplexMessage>(new ComplexMessage{10, {"WUT?"}})}});
    QByteArray result = test.serialize();

    ASSERT_STREQ(result.toHex().toStdString().c_str(),
                "3a180d0a0000001211120d320b74656e207369787465656e08103a230d2a000000121c12183216666f757274792074776f2074656e207369787465656e080a3a110d13000100120a120632045755543f080a");
}

TEST_F(SerializationTest, SimpleSFixed32ComplexMapSerializeTest)
{
    SimpleSFixed32ComplexMessageMapMessage test;
    test.setMapField({{10, QSharedPointer<ComplexMessage>(new ComplexMessage{16 , {"ten sixteen"}})}, {-42, QSharedPointer<ComplexMessage>(new ComplexMessage{10 , {"minus fourty two ten sixteen"}})}, {65555, QSharedPointer<ComplexMessage>(new ComplexMessage{10 , {"WUT?"}})}});
    QByteArray result = test.serialize();

    ASSERT_STREQ(result.toHex().toStdString().c_str(),
                "4a290dd6ffffff1222121e321c6d696e757320666f757274792074776f2074656e207369787465656e080a4a180d0a0000001211120d320b74656e207369787465656e08104a110d13000100120a120632045755543f080a");
}

TEST_F(SerializationTest, SimpleInt32ComplexMapSerializeTest)
{
    SimpleInt32ComplexMessageMapMessage test;
    test.setMapField({{10, QSharedPointer<ComplexMessage>(new ComplexMessage{16 , {"ten sixteen"}})}, {-42, QSharedPointer<ComplexMessage>(new ComplexMessage{10 , {"minus fourty two ten sixteen"}})}, {65555, QSharedPointer<ComplexMessage>(new ComplexMessage{10 , {"WUT?"}})}});
    QByteArray result = test.serialize();

    ASSERT_STREQ(result.toHex().toStdString().c_str(),
                "1a2a08d6ffffff0f1222121e321c6d696e757320666f757274792074776f2074656e207369787465656e080a1a15080a1211120d320b74656e207369787465656e08101a1008938004120a120632045755543f080a");
}

TEST_F(SerializationTest, SimpleSInt32ComplexMapSerializeTest)
{
    SimpleSInt32ComplexMessageMapMessage test;
    test.setMapField({{10, QSharedPointer<ComplexMessage>(new ComplexMessage{16 , {"ten sixteen"}})}, {42, QSharedPointer<ComplexMessage>(new ComplexMessage{10 , {"fourty two ten sixteen"}})}, {-65555, QSharedPointer<ComplexMessage>(new ComplexMessage{10 , {"minus WUT?"}})}});
    QByteArray result = test.serialize();

    ASSERT_STREQ(result.toHex().toStdString().c_str(),
                "0a1608a580081210120c320a6d696e7573205755543f080a0a1508141211120d320b74656e207369787465656e08100a200854121c12183216666f757274792074776f2074656e207369787465656e080a");
}

TEST_F(SerializationTest, SimpleUInt32ComplexMapSerializeTest)
{
    SimpleUInt32ComplexMessageMapMessage test;
    test.setMapField({{10, QSharedPointer<ComplexMessage>(new ComplexMessage{16 , {"ten sixteen"}})}, {42, QSharedPointer<ComplexMessage>(new ComplexMessage{10 , {"fourty two ten sixteen"}})}, {65555, QSharedPointer<ComplexMessage>(new ComplexMessage{10 , {"WUT?"}})}});
    QByteArray result = test.serialize();

    ASSERT_STREQ(result.toHex().toStdString().c_str(),
                "2a15080a1211120d320b74656e207369787465656e08102a20082a121c12183216666f757274792074776f2074656e207369787465656e080a2a1008938004120a120632045755543f080a");
}

TEST_F(SerializationTest, SimpleFixed64ComplexMapSerializeTest)
{
    SimpleFixed64ComplexMessageMapMessage test;
    test.setMapField({{10, QSharedPointer<ComplexMessage>(new ComplexMessage{16 , {"ten sixteen"}})}, {UINT64_MAX, QSharedPointer<ComplexMessage>(new ComplexMessage{42 , {"minus fourty two ten MAAAX"}})}, {65555, QSharedPointer<ComplexMessage>(new ComplexMessage{10 , {"WUT?"}})}});
    QByteArray result = test.serialize();

    ASSERT_STREQ(result.toHex().toStdString().c_str(),
                "421c090a000000000000001211120d320b74656e207369787465656e08104215091300010000000000120a120632045755543f080a422b09ffffffffffffffff1220121c321a6d696e757320666f757274792074776f2074656e204d41414158082a");
}

TEST_F(SerializationTest, SimpleSFixed64ComplexMapSerializeTest)
{
    SimpleSFixed64ComplexMessageMapMessage test;
    test.setMapField({{10, QSharedPointer<ComplexMessage>(new ComplexMessage{16 , {"ten sixteen"}})}, {-42, QSharedPointer<ComplexMessage>(new ComplexMessage{10 , {"minus fourty two ten sixteen"}})}, {65555, QSharedPointer<ComplexMessage>(new ComplexMessage{10 , {"WUT?"}})}});
    QByteArray result = test.serialize();

    ASSERT_STREQ(result.toHex().toStdString().c_str(),
                "522d09d6ffffffffffffff1222121e321c6d696e757320666f757274792074776f2074656e207369787465656e080a521c090a000000000000001211120d320b74656e207369787465656e08105215091300010000000000120a120632045755543f080a");
}

TEST_F(SerializationTest, SimpleInt64ComplexMapSerializeTest)
{
    SimpleInt64ComplexMessageMapMessage test;
    test.setMapField({{10, QSharedPointer<ComplexMessage>(new ComplexMessage{16 , {"ten sixteen"}})}, {-42, QSharedPointer<ComplexMessage>(new ComplexMessage{10 , {"minus fourty two ten sixteen"}})}, {65555, QSharedPointer<ComplexMessage>(new ComplexMessage{10 , {"WUT?"}})}});
    QByteArray result = test.serialize();

    ASSERT_STREQ(result.toHex().toStdString().c_str(),
                "222f08d6ffffffffffffffff011222121e321c6d696e757320666f757274792074776f2074656e207369787465656e080a2215080a1211120d320b74656e207369787465656e0810221008938004120a120632045755543f080a");
}

TEST_F(SerializationTest, SimpleSInt64ComplexMapSerializeTest)
{
    SimpleSInt64ComplexMessageMapMessage test;
    test.setMapField({{10, QSharedPointer<ComplexMessage>(new ComplexMessage{16 , {"ten sixteen"}})}, {-42, QSharedPointer<ComplexMessage>(new ComplexMessage{10 , {"minus fourty two ten sixteen"}})}, {65555, QSharedPointer<ComplexMessage>(new ComplexMessage{10 , {"WUT?"}})}});
    QByteArray result = test.serialize();
    ASSERT_STREQ(result.toHex().toStdString().c_str(),
                "122608531222121e321c6d696e757320666f757274792074776f2074656e207369787465656e080a121508141211120d320b74656e207369787465656e0810121008a68008120a120632045755543f080a");
}

TEST_F(SerializationTest, SimpleUInt64ComplexMapSerializeTest)
{
    SimpleUInt64ComplexMessageMapMessage test;
    test.setMapField({{10, QSharedPointer<ComplexMessage>(new ComplexMessage{11 , {"ten eleven"}})}, {42, QSharedPointer<ComplexMessage>(new ComplexMessage{10 , {"fourty two ten sixteen"}})}, {65555, QSharedPointer<ComplexMessage>(new ComplexMessage{10 , {"WUT?"}})}});
    QByteArray result = test.serialize();

    ASSERT_STREQ(result.toHex().toStdString().c_str(),
                "3214080a1210120c320a74656e20656c6576656e080b3220082a121c12183216666f757274792074776f2074656e207369787465656e080a321008938004120a120632045755543f080a");
}

TEST_F(SerializationTest, SimpleStringComplexMapSerializeTest)
{
    SimpleStringComplexMessageMapMessage test;
    test.setMapField({{"ben", QSharedPointer<ComplexMessage>(new ComplexMessage{11 , {"ten eleven"}})}, {"where is my car dude?", QSharedPointer<ComplexMessage>(new ComplexMessage{10 , {"fourty two ten sixteen"}})}, {"WUT??", QSharedPointer<ComplexMessage>(new ComplexMessage{10 , {"?WUT?"}})}});
    QByteArray result = test.serialize();

    ASSERT_STREQ(result.toHex().toStdString().c_str(),
                "6a140a055755543f3f120b120732053f5755543f080a6a170a0362656e1210120c320a74656e20656c6576656e080b6a350a157768657265206973206d792063617220647564653f121c12183216666f757274792074776f2074656e207369787465656e080a");
}
