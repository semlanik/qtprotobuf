/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>, Viktor Kopp <vifactor@gmail.com>
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

#include "deserializationtest.h"

#include "qtprotobufnamespace/tests/simpletest.qpb.h"

using namespace qtprotobufnamespace::tests;
using namespace QtProtobuf::tests;
using namespace QtProtobuf;

void DeserializationTest::SetUpTestCase()
{
    //Register all types
    QtProtobuf::qRegisterProtobufTypes();
}

void DeserializationTest::SetUp()
{
    serializer.reset(new QProtobufSerializer);
}

TEST_F(DeserializationTest, FixedInt32MessageDeserializeTest)
{
    SimpleFixedInt32Message test;
    test.deserialize(serializer.get(), QByteArray::fromHex("0d0f000000"));
    ASSERT_EQ(test.testFieldFixedInt32(), 15);

    test.deserialize(serializer.get(), QByteArray::fromHex("0d2c010000"));
    ASSERT_EQ(test.testFieldFixedInt32(), 300);

    test.deserialize(serializer.get(), QByteArray::fromHex("0d09000100"));
    ASSERT_EQ(test.testFieldFixedInt32(), 65545);

    test.deserialize(serializer.get(), QByteArray::fromHex("0d00000000"));
    ASSERT_EQ(test.testFieldFixedInt32(), 0);

    test.deserialize(serializer.get(), QByteArray::fromHex("0d2c010000"));
    ASSERT_EQ(test.testFieldFixedInt32(), 300);

    test.deserialize(serializer.get(), QByteArray::fromHex("0d00010000"));
    ASSERT_EQ(test.testFieldFixedInt32(), UINT8_MAX + 1);

    test.deserialize(serializer.get(), QByteArray::fromHex("0d00000100"));
    ASSERT_EQ(test.testFieldFixedInt32(), UINT16_MAX + 1);

    test.deserialize(serializer.get(), QByteArray::fromHex("0dff000000"));
    ASSERT_EQ(test.testFieldFixedInt32(), UINT8_MAX);

    test.deserialize(serializer.get(), QByteArray::fromHex("0dffff0000"));
    ASSERT_EQ(test.testFieldFixedInt32(), UINT16_MAX);

    test.deserialize(serializer.get(), QByteArray::fromHex("0dffffffff"));
    ASSERT_EQ(test.testFieldFixedInt32(), UINT32_MAX);
}

TEST_F(DeserializationTest, FixedInt64MessageDeserializeTest)
{
    SimpleFixedInt64Message test;
    test.deserialize(serializer.get(), QByteArray::fromHex("090f00000000000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), 15);


    test.deserialize(serializer.get(), QByteArray::fromHex("092c01000000000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), 300);

    test.deserialize(serializer.get(), QByteArray::fromHex("090900010000000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), 65545);

    test.deserialize(serializer.get(), QByteArray::fromHex("090000000000000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), 0);

    test.deserialize(serializer.get(), QByteArray::fromHex("090001000000000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), UINT8_MAX + 1);

    test.deserialize(serializer.get(), QByteArray::fromHex("090000010000000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), UINT16_MAX + 1);

    test.deserialize(serializer.get(), QByteArray::fromHex("090000000001000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), (unsigned long long)(UINT32_MAX) + 1);

    test.deserialize(serializer.get(), QByteArray::fromHex("09ff00000000000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), UINT8_MAX);

    test.deserialize(serializer.get(), QByteArray::fromHex("09ffff000000000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), UINT16_MAX);

    test.deserialize(serializer.get(), QByteArray::fromHex("09ffffffff00000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), UINT32_MAX);

    test.deserialize(serializer.get(), QByteArray::fromHex("09ffffffffffffffff"));
    ASSERT_EQ(test.testFieldFixedInt64(), UINT64_MAX);
}

TEST_F(DeserializationTest, SFixedInt32MessageDeserializeTest)
{
    SimpleSFixedInt32Message test;
    test.deserialize(serializer.get(), QByteArray::fromHex("0d0f000000"));
    ASSERT_EQ(test.testFieldFixedInt32(), 15);

    test.deserialize(serializer.get(), QByteArray::fromHex("0d2c010000"));
    ASSERT_EQ(test.testFieldFixedInt32(), 300);

    test.deserialize(serializer.get(), QByteArray::fromHex("0d09000100"));
    ASSERT_EQ(test.testFieldFixedInt32(), 65545);

    test.deserialize(serializer.get(), QByteArray::fromHex("0d00000000"));
    ASSERT_EQ(test.testFieldFixedInt32(), 0);

    test.deserialize(serializer.get(), QByteArray::fromHex("0d2c010000"));
    ASSERT_EQ(test.testFieldFixedInt32(), 300);

    test.deserialize(serializer.get(), QByteArray::fromHex("0d80000000"));
    ASSERT_EQ(test.testFieldFixedInt32(), INT8_MAX + 1);

    test.deserialize(serializer.get(), QByteArray::fromHex("0d00800000"));
    ASSERT_EQ(test.testFieldFixedInt32(), INT16_MAX + 1);

    test.deserialize(serializer.get(), QByteArray::fromHex("0d7f000000"));
    ASSERT_EQ(test.testFieldFixedInt32(), INT8_MAX);

    test.deserialize(serializer.get(), QByteArray::fromHex("0dff7f0000"));
    ASSERT_EQ(test.testFieldFixedInt32(), INT16_MAX);

    test.deserialize(serializer.get(), QByteArray::fromHex("0dffffff7f"));
    ASSERT_EQ(test.testFieldFixedInt32(), INT32_MAX);

    test.deserialize(serializer.get(), QByteArray::fromHex("0d7fffffff"));
    ASSERT_EQ(test.testFieldFixedInt32(), INT8_MIN - 1);

    test.deserialize(serializer.get(), QByteArray::fromHex("0dff7fffff"));
    ASSERT_EQ(test.testFieldFixedInt32(), INT16_MIN - 1);

    test.deserialize(serializer.get(), QByteArray::fromHex("0d80ffffff"));
    ASSERT_EQ(test.testFieldFixedInt32(), INT8_MIN);

    test.deserialize(serializer.get(), QByteArray::fromHex("0d0080ffff"));
    ASSERT_EQ(test.testFieldFixedInt32(), INT16_MIN);

    test.deserialize(serializer.get(), QByteArray::fromHex("0d00000080"));
    ASSERT_EQ(test.testFieldFixedInt32(), INT32_MIN);
}

TEST_F(DeserializationTest, SFixedInt64MessageDeserializeTest)
{
    SimpleSFixedInt64Message test;
    test.deserialize(serializer.get(), QByteArray::fromHex("090f00000000000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), 15);


    test.deserialize(serializer.get(), QByteArray::fromHex("092c01000000000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), 300);

    test.deserialize(serializer.get(), QByteArray::fromHex("090900010000000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), 65545);

    test.deserialize(serializer.get(), QByteArray::fromHex("090000000000000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), 0);

    test.deserialize(serializer.get(), QByteArray::fromHex("098000000000000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), INT8_MAX + 1);

    test.deserialize(serializer.get(), QByteArray::fromHex("090080000000000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), INT16_MAX + 1);

    test.deserialize(serializer.get(), QByteArray::fromHex("090000008000000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), (unsigned long long)(INT32_MAX) + 1);

    test.deserialize(serializer.get(), QByteArray::fromHex("097f00000000000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), INT8_MAX);

    test.deserialize(serializer.get(), QByteArray::fromHex("09ff7f000000000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), INT16_MAX);

    test.deserialize(serializer.get(), QByteArray::fromHex("09ffffff7f00000000"));
    ASSERT_EQ(test.testFieldFixedInt64(), INT32_MAX);

    test.deserialize(serializer.get(), QByteArray::fromHex("09ffffffffffffff7f"));
    ASSERT_EQ(test.testFieldFixedInt64(), INT64_MAX);

    test.deserialize(serializer.get(), QByteArray::fromHex("097fffffffffffffff"));
    ASSERT_EQ(test.testFieldFixedInt64(), INT8_MIN - 1);

    test.deserialize(serializer.get(), QByteArray::fromHex("09ff7fffffffffffff"));
    ASSERT_EQ(test.testFieldFixedInt64(), INT16_MIN - 1);

    test.deserialize(serializer.get(), QByteArray::fromHex("09ffffff7fffffffff"));
    ASSERT_EQ(test.testFieldFixedInt64(), (qlonglong)INT32_MIN - 1);

    test.deserialize(serializer.get(), QByteArray::fromHex("0980ffffffffffffff"));
    ASSERT_EQ(test.testFieldFixedInt64(), INT8_MIN);

    test.deserialize(serializer.get(), QByteArray::fromHex("090080ffffffffffff"));
    ASSERT_EQ(test.testFieldFixedInt64(), INT16_MIN);

    test.deserialize(serializer.get(), QByteArray::fromHex("0900000080ffffffff"));
    ASSERT_EQ(test.testFieldFixedInt64(), INT32_MIN);

    test.deserialize(serializer.get(), QByteArray::fromHex("090000000000000080"));
    ASSERT_EQ(test.testFieldFixedInt64(), INT64_MIN);
}

TEST_F(DeserializationTest, FloatMessageDeserializeTest)
{
    SimpleFloatMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("3dcdcccc3d"));
    ASSERT_FLOAT_EQ(test.testFieldFloat(), 0.1f);

    test.deserialize(serializer.get(), QByteArray::fromHex("3d00008000"));
    ASSERT_FLOAT_EQ(test.testFieldFloat(), FLT_MIN);

    test.deserialize(serializer.get(), QByteArray::fromHex("3dffff7f7f"));
    ASSERT_FLOAT_EQ(test.testFieldFloat(), FLT_MAX);

    test.deserialize(serializer.get(), QByteArray::fromHex("3d666686c0"));
    ASSERT_FLOAT_EQ(test.testFieldFloat(), -4.2f);

    test.deserialize(serializer.get(), QByteArray::fromHex("3d00000000"));
    ASSERT_FLOAT_EQ(test.testFieldFloat(), -0.0f);
}

TEST_F(DeserializationTest, DoubleMessageDeserializeTest)
{
    SimpleDoubleMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("419a9999999999b93f"));
    ASSERT_DOUBLE_EQ(0.1, test.testFieldDouble());

    test.deserialize(serializer.get(), QByteArray::fromHex("410000000000001000"));
    ASSERT_DOUBLE_EQ(DBL_MIN, test.testFieldDouble());

    test.deserialize(serializer.get(), QByteArray::fromHex("41ffffffffffffef7f"));
    ASSERT_DOUBLE_EQ(DBL_MAX, test.testFieldDouble());

    test.deserialize(serializer.get(), QByteArray::fromHex("41cdcccccccccc10c0"));
    ASSERT_DOUBLE_EQ(-4.2, test.testFieldDouble());

    test.deserialize(serializer.get(), QByteArray::fromHex("410000000000000000"));
    ASSERT_DOUBLE_EQ(0.0, test.testFieldDouble());
}

TEST_F(DeserializationTest, IntMessageDeserializeTest)
{
    SimpleIntMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("080f"));
    ASSERT_EQ(15, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("08ac02"));
    ASSERT_EQ(300, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("08898004"));
    ASSERT_EQ(65545, test.testFieldInt());

    //FIXME: bug#68
    //test.deserialize(serializer.get(), QByteArray::fromHex(""));
    //ASSERT_EQ(0, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("088001"));
    ASSERT_EQ(INT8_MAX + 1, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("08808002"));
    ASSERT_EQ(INT16_MAX + 1, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("087f"));
    ASSERT_EQ(INT8_MAX, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("08ffff01"));
    ASSERT_EQ(INT16_MAX, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("08ffffffff07"));
    ASSERT_EQ(INT32_MAX, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("08ffffffffffffffffff01"));
    ASSERT_EQ(-1, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("08b2fcffffffffffffff01"));
    ASSERT_EQ(-462, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("089f8ffcffffffffffff01"));
    ASSERT_EQ(-63585, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("0880ffffffffffffffff01"));
    ASSERT_EQ(INT8_MIN, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("088080feffffffffffff01"));
    ASSERT_EQ(INT16_MIN, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("0880808080f8ffffffff01"));
    ASSERT_EQ(INT32_MIN, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("08fffeffffffffffffff01"));
    ASSERT_EQ(INT8_MIN - 1, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("08fffffdffffffffffff01"));
    ASSERT_EQ(INT16_MIN - 1, test.testFieldInt());
}

TEST_F(DeserializationTest, StringMessageDeserializeTest)
{
    SimpleStringMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("3206717765727479"));
    ASSERT_STREQ("qwerty", test.testFieldString().toStdString().c_str());

    test.deserialize(serializer.get(), QByteArray::fromHex("3280046f6570534e4c4956473038554a706b3257374a74546b6b4278794b303658306c51364d4c37494d6435354b3858433154707363316b4457796d3576387a3638623446517570394f393551536741766a48494131354f583642753638657362514654394c507a5341444a367153474254594248583551535a67333274724364484d6a383058754448717942674d34756636524b71326d675762384f76787872304e774c786a484f66684a384d726664325237686255676a65737062596f5168626748456a32674b45563351766e756d596d7256586531426b437a5a684b56586f6444686a304f6641453637766941793469334f6167316872317a34417a6f384f3558713638504f455a3143735a506f3244584e4e52386562564364594f7a3051364a4c50536c356a61734c434672514e374569564e6a516d437253735a4852674c4e796c76676f454678475978584a39676d4b346d72304f47645a63474a4f5252475a4f514370514d68586d68657a46616c4e494a584d50505861525658695268524150434e55456965384474614357414d717a346e4e5578524d5a355563584258735850736879677a6b7979586e4e575449446f6a466c7263736e4b71536b5131473645383567535a6274495942683773714f36474458486a4f72585661564356435575626a634a4b54686c79736c7432397a48754973354a47707058785831"));
    ASSERT_STREQ("oepSNLIVG08UJpk2W7JtTkkBxyK06X0lQ6ML7IMd55K8XC1Tpsc1kDWym5v8z68b4FQup9O95QSgAvjHIA15OX6Bu68esbQFT9LPzSADJ6qSGBTYBHX5QSZg32trCdHMj80XuDHqyBgM4uf6RKq2mgWb8Ovxxr0NwLxjHOfhJ8Mrfd2R7hbUgjespbYoQhbgHEj2gKEV3QvnumYmrVXe1BkCzZhKVXodDhj0OfAE67viAy4i3Oag1hr1z4Azo8O5Xq68POEZ1CsZPo2DXNNR8ebVCdYOz0Q6JLPSl5jasLCFrQN7EiVNjQmCrSsZHRgLNylvgoEFxGYxXJ9gmK4mr0OGdZcGJORRGZOQCpQMhXmhezFalNIJXMPPXaRVXiRhRAPCNUEie8DtaCWAMqz4nNUxRMZ5UcXBXsXPshygzkyyXnNWTIDojFlrcsnKqSkQ1G6E85gSZbtIYBh7sqO6GDXHjOrXVaVCVCUubjcJKThlyslt29zHuIs5JGppXxX1", test.testFieldString().toStdString().c_str());
}

TEST_F(DeserializationTest, ComplexTypeDeserializeTest)
{
    ComplexMessage test;

    test.deserialize(serializer.get(), QByteArray::fromHex("1208320671776572747908d3ffffffffffffffff01"));
    ASSERT_EQ(-45, test.testFieldInt());
    ASSERT_TRUE(QString::fromUtf8("qwerty") == test.testComplexField().testFieldString());

    test.deserialize(serializer.get(), QByteArray::fromHex("08d3ffffffffffffffff0112083206717765727479"));
    ASSERT_EQ(-45, test.testFieldInt());
    ASSERT_TRUE(QString::fromUtf8("qwerty") == test.testComplexField().testFieldString());

    test.deserialize(serializer.get(), QByteArray::fromHex("128404328104595652664a766a78716267764677533159764f5a5867746a356666474c533741694e487a396f5a496f4b626d377a3848373978427579506b70515876476f4f30394f5939785261777833654f417339786a6f544131784a68727732385441637131436562596c43395755665143366849616e74614e647948694b546f666669305a74376c613432535278585a53503447757862635a49703533704a6e79437766437931716446637a5430646d6e3768386670794164656d456176774665646134643050417047665355326a4c74333958386b595542784e4d325767414c524267486456646538377136506935553639546a684d6432385731534644314478796f67434372714f6374325a5049436f4c6e72716446334f644e7a6a52564c6665797651384c674c76524e4652395766574179417a37396e4b6742616d64384e746c7674344d6733354535675653326737415137726b6d37326342646e5739734345794761626558417548356a34475262754c543771425a574463464c463453734364533357664647644e48667761696a7a796b42796f3731507646566c54584832574a576f4676523546414c6a42546e37624364503070416953624c435938587a324d73633364426235466639474953506255704e6d557642644d5a4d485176714f6d544e584550704e306237344d444f4d5166574a53684f6f334e6b41764d6a73082a"));
    ASSERT_EQ(42, test.testFieldInt());
    ASSERT_TRUE(QString::fromUtf8("YVRfJvjxqbgvFwS1YvOZXgtj5ffGLS7AiNHz9oZIoKbm7z8H79xBuyPkpQXvGoO09OY9xRawx3eOAs9xjoTA1xJhrw28TAcq1CebYlC9WUfQC6hIantaNdyHiKToffi0Zt7la42SRxXZSP4GuxbcZIp53pJnyCwfCy1qdFczT0dmn7h8fpyAdemEavwFeda4d0PApGfSU2jLt39X8kYUBxNM2WgALRBgHdVde87q6Pi5U69TjhMd28W1SFD1DxyogCCrqOct2ZPICoLnrqdF3OdNzjRVLfeyvQ8LgLvRNFR9WfWAyAz79nKgBamd8Ntlvt4Mg35E5gVS2g7AQ7rkm72cBdnW9sCEyGabeXAuH5j4GRbuLT7qBZWDcFLF4SsCdS3WfFGdNHfwaijzykByo71PvFVlTXH2WJWoFvR5FALjBTn7bCdP0pAiSbLCY8Xz2Msc3dBb5Ff9GISPbUpNmUvBdMZMHQvqOmTNXEPpN0b74MDOMQfWJShOo3NkAvMjs") == test.testComplexField().testFieldString());

    test.deserialize(serializer.get(), QByteArray::fromHex("082a128404328104595652664a766a78716267764677533159764f5a5867746a356666474c533741694e487a396f5a496f4b626d377a3848373978427579506b70515876476f4f30394f5939785261777833654f417339786a6f544131784a68727732385441637131436562596c43395755665143366849616e74614e647948694b546f666669305a74376c613432535278585a53503447757862635a49703533704a6e79437766437931716446637a5430646d6e3768386670794164656d456176774665646134643050417047665355326a4c74333958386b595542784e4d325767414c524267486456646538377136506935553639546a684d6432385731534644314478796f67434372714f6374325a5049436f4c6e72716446334f644e7a6a52564c6665797651384c674c76524e4652395766574179417a37396e4b6742616d64384e746c7674344d6733354535675653326737415137726b6d37326342646e5739734345794761626558417548356a34475262754c543771425a574463464c463453734364533357664647644e48667761696a7a796b42796f3731507646566c54584832574a576f4676523546414c6a42546e37624364503070416953624c435938587a324d73633364426235466639474953506255704e6d557642644d5a4d485176714f6d544e584550704e306237344d444f4d5166574a53684f6f334e6b41764d6a73"));
    ASSERT_EQ(42, test.testFieldInt());
    ASSERT_TRUE(QString::fromUtf8("YVRfJvjxqbgvFwS1YvOZXgtj5ffGLS7AiNHz9oZIoKbm7z8H79xBuyPkpQXvGoO09OY9xRawx3eOAs9xjoTA1xJhrw28TAcq1CebYlC9WUfQC6hIantaNdyHiKToffi0Zt7la42SRxXZSP4GuxbcZIp53pJnyCwfCy1qdFczT0dmn7h8fpyAdemEavwFeda4d0PApGfSU2jLt39X8kYUBxNM2WgALRBgHdVde87q6Pi5U69TjhMd28W1SFD1DxyogCCrqOct2ZPICoLnrqdF3OdNzjRVLfeyvQ8LgLvRNFR9WfWAyAz79nKgBamd8Ntlvt4Mg35E5gVS2g7AQ7rkm72cBdnW9sCEyGabeXAuH5j4GRbuLT7qBZWDcFLF4SsCdS3WfFGdNHfwaijzykByo71PvFVlTXH2WJWoFvR5FALjBTn7bCdP0pAiSbLCY8Xz2Msc3dBb5Ff9GISPbUpNmUvBdMZMHQvqOmTNXEPpN0b74MDOMQfWJShOo3NkAvMjs") == test.testComplexField().testFieldString());
}

TEST_F(DeserializationTest, RepeatedStringMessageTest)
{
    RepeatedStringMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("0a04616161610a0562626262620a036363630a066464646464640a056565656565"));
    ASSERT_EQ(5, test.testRepeatedString().count());
    ASSERT_TRUE(test.testRepeatedString() == QStringList({"aaaa","bbbbb","ccc","dddddd","eeeee"}));
}

TEST_F(DeserializationTest, RepeatedBytesMessageTest)
{
    RepeatedBytesMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("0a060102030405060a04ffffffff0a05eaeaeaeaea0a06010203040506"));
    ASSERT_EQ(4, test.testRepeatedBytes().count());
    ASSERT_TRUE(test.testRepeatedBytes() == QByteArrayList({QByteArray::fromHex("010203040506"),
                                                             QByteArray::fromHex("ffffffff"),
                                                             QByteArray::fromHex("eaeaeaeaea"),
                                                             QByteArray::fromHex("010203040506")}));
    //TODO: Serialization for list types works partially incorrect because of appending of values to existing
    //Need to make decision if deserialize should reset all protobuf properties or not
    RepeatedBytesMessage test2;
    test2.deserialize(serializer.get(), QByteArray::fromHex("0a060102030405060a000a05eaeaeaeaea0a06010203040506"));
    ASSERT_EQ(4, test2.testRepeatedBytes().count());
    ASSERT_TRUE(test2.testRepeatedBytes() == QByteArrayList({QByteArray::fromHex("010203040506"),
                                                             QByteArray::fromHex(""),
                                                             QByteArray::fromHex("eaeaeaeaea"),
                                                             QByteArray::fromHex("010203040506")}));
}

TEST_F(DeserializationTest, RepeatedFloatMessageTest)
{
    RepeatedFloatMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("0a14cdcccc3e9a99993f0000003f3333b33f9a99193f"));
    ASSERT_EQ(5, test.testRepeatedFloat().count());
    ASSERT_TRUE(test.testRepeatedFloat() == FloatList({0.4f, 1.2f, 0.5f, 1.4f, 0.6f}));
}

TEST_F(DeserializationTest, RepeatedDoubleMessageTest)
{
    RepeatedDoubleMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("0a289a9999999999b93f9a9999999999c93f333333333333d33f9a9999999999d93f000000000000e03f"));
    ASSERT_EQ(5, test.testRepeatedDouble().count());
    ASSERT_TRUE(test.testRepeatedDouble() == DoubleList({0.1, 0.2, 0.3, 0.4, 0.5}));
}

TEST_F(DeserializationTest, RepeatedIntMessageTest)
{
    RepeatedIntMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("0a1101c102b1fcfbff0fedc207fdffffff0f03"));
    ASSERT_EQ(6, test.testRepeatedInt().count());
    ASSERT_TRUE(test.testRepeatedInt() == int32List({1, 321, -65999, 123245, -3, 3}));

    RepeatedIntMessage test2;
    test2.deserialize(serializer.get(), QByteArray::fromHex("0a1b01c102b1fcfbffffffffffff01edc207fdffffffffffffffff0103"));
    ASSERT_EQ(6, test2.testRepeatedInt().count());
    ASSERT_TRUE(test2.testRepeatedInt() == int32List({1, 321, -65999, 123245, -3, 3}));
}

TEST_F(DeserializationTest, RepeatedSIntMessageTest)
{
    RepeatedSIntMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("0a0b0282059d8708da850f0506"));
    ASSERT_EQ(6, test.testRepeatedInt().count());
    ASSERT_TRUE(test.testRepeatedInt() == sint32List({1, 321, -65999, 123245, -3, 3}));
}

TEST_F(DeserializationTest, RepeatedUIntMessageTest)
{
    RepeatedUIntMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("0a0a01c102cf8304edc20703"));
    ASSERT_EQ(5, test.testRepeatedInt().count());
    ASSERT_TRUE(test.testRepeatedInt() == uint32List({1, 321, 65999, 123245, 3}));
}

TEST_F(DeserializationTest, RepeatedInt64MessageTest)
{
    RepeatedInt64Message test;
    test.deserialize(serializer.get(), QByteArray::fromHex("0a1f01c102b1fcfbffffffffffff01b3c3cab6d8e602fdffffffffffffffff0103"));
    ASSERT_EQ(6, test.testRepeatedInt().count());
    ASSERT_TRUE(test.testRepeatedInt() == int64List({1, 321, -65999, 12324523123123, -3, 3}));
}

TEST_F(DeserializationTest, RepeatedSInt64MessageTest)
{
    RepeatedSInt64Message test;
    test.deserialize(serializer.get(), QByteArray::fromHex("0a0f0282059d8708e68695edb0cd050506"));
    ASSERT_EQ(6, test.testRepeatedInt().count());
    ASSERT_TRUE(test.testRepeatedInt() == sint64List({1, 321, -65999, 12324523123123, -3, 3}));
}

TEST_F(DeserializationTest, RepeatedUInt64MessageTest)
{
    RepeatedUInt64Message test;
    test.deserialize(serializer.get(), QByteArray::fromHex("0a1301c102cf8304edc207d28b9fda82dff6da0103"));
    ASSERT_EQ(6, test.testRepeatedInt().count());
    ASSERT_TRUE(test.testRepeatedInt() == uint64List({1, 321, 65999, 123245, 123245324235425234, 3}));
}

TEST_F(DeserializationTest, RepeatedFixedIntMessageTest)
{
    RepeatedFixedIntMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("0a180100000041010000cf010100ab0ebc000300000003000000"));
    ASSERT_EQ(6, test.testRepeatedInt().count());
    ASSERT_TRUE(test.testRepeatedInt() == fixed32List({1, 321, 65999, 12324523, 3, 3}));
}

TEST_F(DeserializationTest, RepeatedSFixedIntMessageTest)
{
    RepeatedSFixedIntMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("0a18010000004101000031fefeffab0ebc00fdffffff03000000"));
    ASSERT_EQ(6, test.testRepeatedInt().count());
    ASSERT_TRUE(test.testRepeatedInt() == sfixed32List({1, 321, -65999, 12324523, -3, 3}));
}

TEST_F(DeserializationTest, RepeatedFixedInt64MessageTest)
{
    RepeatedFixedInt64Message test;
    test.deserialize(serializer.get(), QByteArray::fromHex("0a3001000000000000004101000000000000cf01010000000000d2c5472bf8dab50103000000000000000300000000000000"));
    ASSERT_EQ(6, test.testRepeatedInt().count());
    ASSERT_TRUE(test.testRepeatedInt() == fixed64List({1, 321, 65999, 123245324235425234, 3, 3}));
}

TEST_F(DeserializationTest, RepeatedSFixedInt64MessageTest)
{
    RepeatedSFixedInt64Message test;
    test.deserialize(serializer.get(), QByteArray::fromHex("0a300100000000000000410100000000000031fefeffffffffffd2c5472bf8dab501fdffffffffffffff0300000000000000"));
    ASSERT_EQ(6, test.testRepeatedInt().count());
    ASSERT_TRUE(test.testRepeatedInt() == sfixed64List({1, 321, -65999, 123245324235425234, -3, 3}));
}

TEST_F(DeserializationTest, RepeatedComplexMessageTest)
{
    RepeatedComplexMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("0a0c0819120832067177657274790a0c0819120832067177657274790a0c081912083206717765727479"));
    ASSERT_EQ(3, test.testRepeatedComplex().count());
    ASSERT_EQ(25, test.testRepeatedComplex().at(0)->testFieldInt());
    ASSERT_TRUE(test.testRepeatedComplex().at(0)->testComplexField().testFieldString() == QString("qwerty"));
    ASSERT_EQ(25, test.testRepeatedComplex().at(1)->testFieldInt());
    ASSERT_TRUE(test.testRepeatedComplex().at(1)->testComplexField().testFieldString() == QString("qwerty"));
    ASSERT_EQ(25, test.testRepeatedComplex().at(2)->testFieldInt());
    ASSERT_TRUE(test.testRepeatedComplex().at(2)->testComplexField().testFieldString() == QString("qwerty"));

    //FIXME: This setter should not be called in this test. See bug#69
    test.setTestRepeatedComplex({});
    test.deserialize(serializer.get(), QByteArray::fromHex("0a1508d3feffffffffffffff0112083206717765727479"));
    ASSERT_LT(0, test.testRepeatedComplex().count());
    ASSERT_EQ(-173, test.testRepeatedComplex().at(0)->testFieldInt());
    ASSERT_TRUE(test.testRepeatedComplex().at(0)->testComplexField().testFieldString() == QString("qwerty"));
}

TEST_F(DeserializationTest, SIntMessageDeserializeTest)
{
    SimpleSIntMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("081e"));
    ASSERT_EQ(15, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("08d804"));
    ASSERT_EQ(300, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("08928008"));
    ASSERT_EQ(65545, test.testFieldInt());

    //FIXME: bug#68
    //test.deserialize(serializer.get(), QByteArray::fromHex(""));
    //ASSERT_EQ(0, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("088002"));
    ASSERT_EQ(INT8_MAX + 1, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("08808004"));
    ASSERT_EQ(INT16_MAX + 1, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("08fe01"));
    ASSERT_EQ(INT8_MAX, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("08feff03"));
    ASSERT_EQ(INT16_MAX, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("08feffffff0f"));
    ASSERT_EQ(INT32_MAX, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("0801"));
    ASSERT_EQ(-1, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("089b07"));
    ASSERT_EQ(-462, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("08c1e107"));
    ASSERT_EQ(-63585, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("08ff01"));
    ASSERT_EQ(INT8_MIN, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("08ffff03"));
    ASSERT_EQ(INT16_MIN, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("08ffffffff0f"));
    ASSERT_EQ(INT32_MIN, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("088102"));
    ASSERT_EQ(INT8_MIN - 1, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("08818004"));
    ASSERT_EQ(INT16_MIN - 1, test.testFieldInt());
}

TEST_F(DeserializationTest, UIntMessageDeserializeTest)
{
    SimpleUIntMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("080f"));
    ASSERT_EQ(15, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("08ac02"));
    ASSERT_EQ(300, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("08898004"));
    ASSERT_EQ(65545, test.testFieldInt());

    //FIXME: bug#68
    //test.deserialize(serializer.get(), QByteArray::fromHex(""));
    //ASSERT_EQ(0, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("088002"));
    ASSERT_EQ(UINT8_MAX + 1, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("08808004"));
    ASSERT_EQ(UINT16_MAX + 1, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("08ff01"));
    ASSERT_EQ(UINT8_MAX, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("08ffff03"));
    ASSERT_EQ(UINT16_MAX, test.testFieldInt());

    test.deserialize(serializer.get(), QByteArray::fromHex("08ffffffff0f"));
    ASSERT_EQ(UINT32_MAX, test.testFieldInt());
}

TEST_F(DeserializationTest, BoolDeserializeTest)
{
    SimpleBoolMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("0801"));
    ASSERT_EQ(test.testFieldBool(), true);

    //FIXME: bug#68
    //test.deserialize(serializer.get(), QByteArray::fromHex(""));
    test.deserialize(serializer.get(), QByteArray::fromHex("0800"));
    ASSERT_EQ(test.testFieldBool(), false);
}

TEST_F(DeserializationTest, SimpleEnumMessageDeserializeTest)
{
    SimpleEnumMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("0803"));
    ASSERT_EQ(test.localEnum(), SimpleEnumMessage::LOCAL_ENUM_VALUE3);
}

TEST_F(DeserializationTest, SimpleFixed32StringMapDeserializeTest)
{
    SimpleFixed32StringMapMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("3a0a0d0a000000120374656e3a0e0d0f00000012076669667465656e3a110d2a000000120a666f757274792074776f"));
    ASSERT_TRUE(test.mapField() == SimpleFixed32StringMapMessage::MapFieldEntry({{10, {"ten"}}, {42, {"fourty two"}}, {15, {"fifteen"}}}));
}

TEST_F(DeserializationTest, SimpleSFixed32StringMapDeserializeTest)
{
    SimpleSFixed32StringMapMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("4a170dd6ffffff12106d696e757320666f757274792074776f4a0a0d0a000000120374656e4a0e0d0f00000012076669667465656e"));
    ASSERT_TRUE(test.mapField() == SimpleSFixed32StringMapMessage::MapFieldEntry({{10, {"ten"}}, {-42, {"minus fourty two"}}, {15, {"fifteen"}}}));
}

TEST_F(DeserializationTest, SimpleInt32StringMapDeserializeTest)
{
    SimpleInt32StringMapMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("1a1608f6ffffffffffffffff0112096d696e75732074656e1a0b080f12076669667465656e1a0e082a120a666f757274792074776f"));
    ASSERT_TRUE(test.mapField() == SimpleInt32StringMapMessage::MapFieldEntry({{-10, {"minus ten"}}, {42, {"fourty two"}}, {15, {"fifteen"}}}));
}

TEST_F(DeserializationTest, SimpleSInt32StringMapDeserializeTest)
{
    SimpleSInt32StringMapMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("0a14085312106d696e757320666f757274792074776f0a070814120374656e0a0b081e12076669667465656e"));
    ASSERT_TRUE(test.mapField() == SimpleSInt32StringMapMessage::MapFieldEntry({{10, {"ten"}}, {-42, {"minus fourty two"}}, {15, {"fifteen"}}}));
}

TEST_F(DeserializationTest, SimpleUInt32StringMapDeserializeTest)
{
    SimpleUInt32StringMapMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("2a07080a120374656e2a0b080f12076669667465656e2a0e082a120a666f757274792074776f"));
    ASSERT_TRUE(test.mapField() == SimpleUInt32StringMapMessage::MapFieldEntry({{10, {"ten"}}, {42, {"fourty two"}}, {15, {"fifteen"}}}));
}

TEST_F(DeserializationTest, SimpleFixed64StringMapDeserializeTest)
{
    SimpleFixed64StringMapMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("420e090a00000000000000120374656e4212090f0000000000000012076669667465656e4215092a00000000000000120a666f757274792074776f"));
    ASSERT_TRUE(test.mapField() == SimpleFixed64StringMapMessage::MapFieldEntry({{10, {"ten"}}, {42, {"fourty two"}}, {15, {"fifteen"}}}));
}

TEST_F(DeserializationTest, SimpleSFixed64StringMapDeserializeTest)
{
    SimpleSFixed64StringMapMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("521b09d6ffffffffffffff12106d696e757320666f757274792074776f520e090a00000000000000120374656e5212090f0000000000000012076669667465656e"));
    ASSERT_TRUE(test.mapField() == SimpleSFixed64StringMapMessage::MapFieldEntry({{10, {"ten"}}, {-42, {"minus fourty two"}}, {15, {"fifteen"}}}));
}

TEST_F(DeserializationTest, SimpleInt64StringMapDeserializeTest)
{
    SimpleInt64StringMapMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("221608f6ffffffffffffffff0112096d696e75732074656e220b080f12076669667465656e220e082a120a666f757274792074776f"));
    ASSERT_TRUE(test.mapField() == SimpleInt64StringMapMessage::MapFieldEntry({{-10, {"minus ten"}}, {42, {"fourty two"}}, {15, {"fifteen"}}}));
}

TEST_F(DeserializationTest, SimpleSInt64StringMapDeserializeTest)
{
    SimpleSInt64StringMapMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("1214085312106d696e757320666f757274792074776f12070814120374656e120b081e12076669667465656e"));
    ASSERT_TRUE(test.mapField() == SimpleSInt64StringMapMessage::MapFieldEntry({{10, {"ten"}}, {-42, {"minus fourty two"}}, {15, {"fifteen"}}}));
}

TEST_F(DeserializationTest, SimpleUInt64StringMapDeserializeTest)
{
    SimpleUInt64StringMapMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("3207080a120374656e320b080f12076669667465656e320e082a120a666f757274792074776f"));
    ASSERT_TRUE(test.mapField() == SimpleUInt64StringMapMessage::MapFieldEntry({{10, {"ten"}}, {42, {"fourty two"}}, {15, {"fifteen"}}}));
}

TEST_F(DeserializationTest, SimpleStringStringMapDeserializeTest)
{
    SimpleStringStringMapMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("6a0a0a0362656e120374656e6a100a05737765657412076669667465656e6a210a13776861742069732074686520616e737765723f120a666f757274792074776f"));
    ASSERT_TRUE(test.mapField() == SimpleStringStringMapMessage::MapFieldEntry({{"ben", "ten"}, {"what is the answer?", "fourty two"}, {"sweet", "fifteen"}}));
}

//Complex map
TEST_F(DeserializationTest, SimpleFixed32ComplexMapDeserializeTest)
{
    SimpleFixed32ComplexMessageMapMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("3a180d0a00000012110810120d320b74656e207369787465656e3a230d2a000000121c080a12183216666f757274792074776f2074656e207369787465656e3a110d13000100120a080a120632045755543f"));
    ASSERT_TRUE(*test.mapField()[10].data() == ComplexMessage({16, {"ten sixteen"}}));
    ASSERT_TRUE(*test.mapField()[42].data() == ComplexMessage({10, {"fourty two ten sixteen"}}));
    ASSERT_TRUE(*test.mapField()[65555].data() == ComplexMessage({10, {"WUT?"}}));
}

TEST_F(DeserializationTest, SimpleSFixed32ComplexMapDeserializeTest)
{
    SimpleSFixed32ComplexMessageMapMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("4a290dd6ffffff1222121e321c6d696e757320666f757274792074776f2074656e207369787465656e080a4a180d0a0000001211120d320b74656e207369787465656e08104a110d13000100120a120632045755543f080a"));

    ASSERT_TRUE(*test.mapField()[10] == ComplexMessage({16 , {"ten sixteen"}}));
    ASSERT_TRUE(*test.mapField()[-42] == ComplexMessage({10 , {"minus fourty two ten sixteen"}}));
    ASSERT_TRUE(*test.mapField()[65555] == ComplexMessage({10 , {"WUT?"}}));
}

TEST_F(DeserializationTest, SimpleInt32ComplexMapDeserializeTest)
{
    SimpleInt32ComplexMessageMapMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("1a2f08d6ffffffffffffffff011222121e321c6d696e757320666f757274792074776f2074656e207369787465656e080a1a15080a1211120d320b74656e207369787465656e08101a1008938004120a120632045755543f080a"));
    ASSERT_TRUE(*test.mapField()[10] == ComplexMessage({16 , {"ten sixteen"}}));
    ASSERT_TRUE(*test.mapField()[-42] == ComplexMessage({10 , {"minus fourty two ten sixteen"}}));
    ASSERT_TRUE(*test.mapField()[65555] == ComplexMessage({10 , {"WUT?"}}));
}

TEST_F(DeserializationTest, SimpleSInt32ComplexMapDeserializeTest)
{
    SimpleSInt32ComplexMessageMapMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("0a1608a580081210120c320a6d696e7573205755543f080a0a1508141211120d320b74656e207369787465656e08100a200854121c12183216666f757274792074776f2074656e207369787465656e080a"));
    ASSERT_TRUE(*test.mapField()[10] == ComplexMessage({16 , {"ten sixteen"}}));
    ASSERT_TRUE(*test.mapField()[42] == ComplexMessage({10 , {"fourty two ten sixteen"}}));
    ASSERT_TRUE(*test.mapField()[-65555] == ComplexMessage({10 , {"minus WUT?"}}));
}

TEST_F(DeserializationTest, SimpleUInt32ComplexMapDeserializeTest)
{
    SimpleUInt32ComplexMessageMapMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("2a15080a1211120d320b74656e207369787465656e08102a20082a121c12183216666f757274792074776f2074656e207369787465656e080a2a1008938004120a120632045755543f080a"));
    ASSERT_TRUE(*test.mapField()[10] == ComplexMessage({16 , {"ten sixteen"}}));
    ASSERT_TRUE(*test.mapField()[42] == ComplexMessage({10 , {"fourty two ten sixteen"}}));
    ASSERT_TRUE(*test.mapField()[65555] == ComplexMessage({10 , {"WUT?"}}));
}

TEST_F(DeserializationTest, SimpleFixed64ComplexMapDeserializeTest)
{
    SimpleFixed64ComplexMessageMapMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("421c090a000000000000001211120d320b74656e207369787465656e08104215091300010000000000120a120632045755543f080a422b09ffffffffffffffff1220121c321a6d696e757320666f757274792074776f2074656e204d41414158082a"));

    ASSERT_TRUE(*test.mapField()[10] == ComplexMessage({16 , {"ten sixteen"}}));
    ASSERT_TRUE(*test.mapField()[UINT64_MAX] == ComplexMessage({42 , {"minus fourty two ten MAAAX"}}));
    ASSERT_TRUE(*test.mapField()[65555] == ComplexMessage({10 , {"WUT?"}}));
}

TEST_F(DeserializationTest, SimpleSFixed64ComplexMapDeserializeTest)
{
    SimpleSFixed64ComplexMessageMapMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("522d09d6ffffffffffffff1222121e321c6d696e757320666f757274792074776f2074656e207369787465656e080a521c090a000000000000001211120d320b74656e207369787465656e08105215091300010000000000120a120632045755543f080a"));
    ASSERT_TRUE(*test.mapField()[10] == ComplexMessage({16 , {"ten sixteen"}}));
    ASSERT_TRUE(*test.mapField()[-42] == ComplexMessage({10 , {"minus fourty two ten sixteen"}}));
    ASSERT_TRUE(*test.mapField()[65555] == ComplexMessage({10 , {"WUT?"}}));
}

TEST_F(DeserializationTest, SimpleInt64ComplexMapDeserializeTest)
{
    SimpleInt64ComplexMessageMapMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("222f08d6ffffffffffffffff011222121e321c6d696e757320666f757274792074776f2074656e207369787465656e080a2215080a1211120d320b74656e207369787465656e0810221008938004120a120632045755543f080a"));
    ASSERT_TRUE(*test.mapField()[10] == ComplexMessage({16 , {"ten sixteen"}}));
    ASSERT_TRUE(*test.mapField()[-42] == ComplexMessage({10 , {"minus fourty two ten sixteen"}}));
    ASSERT_TRUE(*test.mapField()[65555] == ComplexMessage({10 , {"WUT?"}}));
}

TEST_F(DeserializationTest, SimpleSInt64ComplexMapDeserializeTest)
{
    SimpleSInt64ComplexMessageMapMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("122608531222121e321c6d696e757320666f757274792074776f2074656e207369787465656e080a121508141211120d320b74656e207369787465656e0810121008a68008120a120632045755543f080a"));
    ASSERT_TRUE(*test.mapField()[10] == ComplexMessage({16 , {"ten sixteen"}}));
    ASSERT_TRUE(*test.mapField()[-42] == ComplexMessage({10 , {"minus fourty two ten sixteen"}}));
    ASSERT_TRUE(*test.mapField()[65555] == ComplexMessage({10 , {"WUT?"}}));
}

TEST_F(DeserializationTest, SimpleUInt64ComplexMapDeserializeTest)
{
    SimpleUInt64ComplexMessageMapMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("3214080a1210120c320a74656e20656c6576656e080b3220082a121c12183216666f757274792074776f2074656e207369787465656e080a321008938004120a120632045755543f080a"));
    ASSERT_TRUE(*test.mapField()[10] == ComplexMessage({11 , {"ten eleven"}}));
    ASSERT_TRUE(*test.mapField()[42] == ComplexMessage({10 , {"fourty two ten sixteen"}}));
    ASSERT_TRUE(*test.mapField()[65555] == ComplexMessage({10 , {"WUT?"}}));
}

TEST_F(DeserializationTest, SimpleStringComplexMapDeserializeTest)
{
    SimpleStringComplexMessageMapMessage test;
    test.deserialize(serializer.get(), QByteArray::fromHex("6a140a055755543f3f120b120732053f5755543f080a6a170a0362656e1210120c320a74656e20656c6576656e080b6a350a157768657265206973206d792063617220647564653f121c12183216666f757274792074776f2074656e207369787465656e080a"));
    ASSERT_TRUE(*test.mapField()["ben"] == ComplexMessage({11 , {"ten eleven"}}));
    ASSERT_TRUE(*test.mapField()["where is my car dude?"] == ComplexMessage({10 , {"fourty two ten sixteen"}}));
    ASSERT_TRUE(*test.mapField()["WUT??"] == ComplexMessage({10 , {"?WUT?"}}));
}

TEST_F(DeserializationTest, SimpleUInt64ComplexMapInvalidLengthDeserializeTest)
{
    SimpleUInt64ComplexMessageMapMessage test;
    EXPECT_THROW(test.deserialize(serializer.get(), QByteArray::fromHex("3214080a1210120c320a74656e20656c6576656e080b3220082a121c12183216666f757274792074776f2074656e207369787465656e080a321008938004120a120")),
                 std::invalid_argument);
    ASSERT_TRUE(test.mapField().isEmpty());
}

TEST_F(DeserializationTest, SimpleStringComplexMapInvalidLengthDeserializeTest)
{
    SimpleStringComplexMessageMapMessage test;
    EXPECT_THROW(test.deserialize(serializer.get(), QByteArray::fromHex("6a140a055755543f3f120b120732053f5755543f080a6a170a0362656e1210120c320a74656e20656c6576656e080b6a350a157768657265206973206d792063617220647564653f121c12183216666f757274792074776f2074656e20736978746565")),
                 std::out_of_range);
    ASSERT_TRUE(*test.mapField()["ben"] == ComplexMessage({11 , {"ten eleven"}}));
    ASSERT_TRUE(*test.mapField()["WUT??"] == ComplexMessage({10 , {"?WUT?"}}));
    ASSERT_FALSE(test.mapField().contains("where is my car dude?"));
}

TEST_F(DeserializationTest, SimpleUInt64ComplexMapCorruptedDeserializeTest)
{
    SimpleUInt64ComplexMessageMapMessage test;
    EXPECT_THROW(test.deserialize(serializer.get(), QByteArray::fromHex("3214080a1210120c320a74656e20656c6576656e080b3221233522345b2183216666f757274792074776f2074656e207369787465656e080a321008938004120a120632045755543f080a")),
                 std::invalid_argument);
    ASSERT_TRUE(test.mapField().isEmpty());
}

TEST_F(DeserializationTest, RedundantFieldIsIgnoredAtDeserializationTest)
{
    ComplexMessage test;
    //3206717765727479 length delimited field number 6
    ASSERT_NO_THROW(test.deserialize(serializer.get(), QByteArray::fromHex("120832067177657274793206717765727479")));
    EXPECT_EQ(test.testFieldInt(), 0);
    EXPECT_STREQ(test.testComplexField().testFieldString().toStdString().c_str(), "qwerty");

    //3dcdcccc3d fixed32 field number 7
    ASSERT_NO_THROW(test.deserialize(serializer.get(), QByteArray::fromHex("120832067177657274793dcdcccc3d")));
    EXPECT_EQ(test.testFieldInt(), 0);
    EXPECT_STREQ(test.testComplexField().testFieldString().toStdString().c_str(), "qwerty");

    //419a9999999999b93f fixed64 field number 8
    ASSERT_NO_THROW(test.deserialize(serializer.get(), QByteArray::fromHex("12083206717765727479419a9999999999b93f")));
    EXPECT_EQ(test.testFieldInt(), 0);
    EXPECT_STREQ(test.testComplexField().testFieldString().toStdString().c_str(), "qwerty");

    //60d3ffffffffffffffff01 varint field number 12
    ASSERT_NO_THROW(test.deserialize(serializer.get(), QByteArray::fromHex("60d3ffffffffffffffff0112083206717765727479")));
    EXPECT_EQ(test.testFieldInt(), 0);
    EXPECT_STREQ(test.testComplexField().testFieldString().toStdString().c_str(), "qwerty");
}

TEST_F(DeserializationTest, FieldIndexRangeTest)
{
    FieldIndexTest1Message msg1(0);
    msg1.deserialize(serializer.get(), QByteArray::fromHex("f80102"));
    ASSERT_EQ(msg1.testField(), 1);

    FieldIndexTest2Message msg2(0);
    msg2.deserialize(serializer.get(), QByteArray::fromHex("f8ff0302"));
    ASSERT_EQ(msg2.testField(), 1);

    FieldIndexTest3Message msg3(0);
    msg3.deserialize(serializer.get(), QByteArray::fromHex("f8ffff0702"));
    ASSERT_EQ(msg3.testField(), 1);

    FieldIndexTest4Message msg4(0);
    msg4.deserialize(serializer.get(), QByteArray::fromHex("f8ffffff0f02"));
    ASSERT_EQ(msg4.testField(), 1);
}

TEST_F(DeserializationTest, SimpleEnumListMessageTest)
{
    SimpleEnumListMessage msg;

    msg.deserialize(serializer.get(), QByteArray());
    ASSERT_TRUE(msg.localEnumList().isEmpty());

    msg.deserialize(serializer.get(), QByteArray::fromHex("0a06000102010203"));
    ASSERT_TRUE((msg.localEnumList() == SimpleEnumListMessage::LocalEnumRepeated {SimpleEnumListMessage::LOCAL_ENUM_VALUE0,
                SimpleEnumListMessage::LOCAL_ENUM_VALUE1,
                SimpleEnumListMessage::LOCAL_ENUM_VALUE2,
                SimpleEnumListMessage::LOCAL_ENUM_VALUE1,
                SimpleEnumListMessage::LOCAL_ENUM_VALUE2,
                SimpleEnumListMessage::LOCAL_ENUM_VALUE3}));
}
