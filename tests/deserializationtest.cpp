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
#include "simplestringmessage.h"
#include "complexmessage.h"
#include "repeatedstringmessage.h"
#include "repeatedbytesmessage.h"
#include "repeateddoublemessage.h"
#include "repeatedfloatmessage.h"
#include "repeatedintmessage.h"

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

TEST_F(DeserializationTest, StringMessageDeserializeTest)
{
    SimpleStringMessage test;
    test.deserialize(QByteArray::fromHex("3206717765727479"));
    ASSERT_STREQ("qwerty", test.testFieldString().toStdString().c_str());

    test.deserialize(QByteArray::fromHex("3280046f6570534e4c4956473038554a706b3257374a74546b6b4278794b303658306c51364d4c37494d6435354b3858433154707363316b4457796d3576387a3638623446517570394f393551536741766a48494131354f583642753638657362514654394c507a5341444a367153474254594248583551535a67333274724364484d6a383058754448717942674d34756636524b71326d675762384f76787872304e774c786a484f66684a384d726664325237686255676a65737062596f5168626748456a32674b45563351766e756d596d7256586531426b437a5a684b56586f6444686a304f6641453637766941793469334f6167316872317a34417a6f384f3558713638504f455a3143735a506f3244584e4e52386562564364594f7a3051364a4c50536c356a61734c434672514e374569564e6a516d437253735a4852674c4e796c76676f454678475978584a39676d4b346d72304f47645a63474a4f5252475a4f514370514d68586d68657a46616c4e494a584d50505861525658695268524150434e55456965384474614357414d717a346e4e5578524d5a355563584258735850736879677a6b7979586e4e575449446f6a466c7263736e4b71536b5131473645383567535a6274495942683773714f36474458486a4f72585661564356435575626a634a4b54686c79736c7432397a48754973354a47707058785831"));
    ASSERT_STREQ("oepSNLIVG08UJpk2W7JtTkkBxyK06X0lQ6ML7IMd55K8XC1Tpsc1kDWym5v8z68b4FQup9O95QSgAvjHIA15OX6Bu68esbQFT9LPzSADJ6qSGBTYBHX5QSZg32trCdHMj80XuDHqyBgM4uf6RKq2mgWb8Ovxxr0NwLxjHOfhJ8Mrfd2R7hbUgjespbYoQhbgHEj2gKEV3QvnumYmrVXe1BkCzZhKVXodDhj0OfAE67viAy4i3Oag1hr1z4Azo8O5Xq68POEZ1CsZPo2DXNNR8ebVCdYOz0Q6JLPSl5jasLCFrQN7EiVNjQmCrSsZHRgLNylvgoEFxGYxXJ9gmK4mr0OGdZcGJORRGZOQCpQMhXmhezFalNIJXMPPXaRVXiRhRAPCNUEie8DtaCWAMqz4nNUxRMZ5UcXBXsXPshygzkyyXnNWTIDojFlrcsnKqSkQ1G6E85gSZbtIYBh7sqO6GDXHjOrXVaVCVCUubjcJKThlyslt29zHuIs5JGppXxX1", test.testFieldString().toStdString().c_str());
}

TEST_F(DeserializationTest, ComplexTypeDeserializeTest)
{
    qRegisterMetaType<SimpleStringMessage>("SimpleStringMessage");
    ComplexMessage test;

    test.deserialize(QByteArray::fromHex("120832067177657274790859"));
    ASSERT_EQ(-45, test.testFieldInt());
    ASSERT_TRUE(QString::fromUtf8("qwerty") == test.testComplexField().testFieldString());

    test.deserialize(QByteArray::fromHex("085912083206717765727479"));
    ASSERT_EQ(-45, test.testFieldInt());
    ASSERT_TRUE(QString::fromUtf8("qwerty") == test.testComplexField().testFieldString());

    test.deserialize(QByteArray::fromHex("128404328104595652664a766a78716267764677533159764f5a5867746a356666474c533741694e487a396f5a496f4b626d377a3848373978427579506b70515876476f4f30394f5939785261777833654f417339786a6f544131784a68727732385441637131436562596c43395755665143366849616e74614e647948694b546f666669305a74376c613432535278585a53503447757862635a49703533704a6e79437766437931716446637a5430646d6e3768386670794164656d456176774665646134643050417047665355326a4c74333958386b595542784e4d325767414c524267486456646538377136506935553639546a684d6432385731534644314478796f67434372714f6374325a5049436f4c6e72716446334f644e7a6a52564c6665797651384c674c76524e4652395766574179417a37396e4b6742616d64384e746c7674344d6733354535675653326737415137726b6d37326342646e5739734345794761626558417548356a34475262754c543771425a574463464c463453734364533357664647644e48667761696a7a796b42796f3731507646566c54584832574a576f4676523546414c6a42546e37624364503070416953624c435938587a324d73633364426235466639474953506255704e6d557642644d5a4d485176714f6d544e584550704e306237344d444f4d5166574a53684f6f334e6b41764d6a730854"));
    ASSERT_EQ(42, test.testFieldInt());
    ASSERT_TRUE(QString::fromUtf8("YVRfJvjxqbgvFwS1YvOZXgtj5ffGLS7AiNHz9oZIoKbm7z8H79xBuyPkpQXvGoO09OY9xRawx3eOAs9xjoTA1xJhrw28TAcq1CebYlC9WUfQC6hIantaNdyHiKToffi0Zt7la42SRxXZSP4GuxbcZIp53pJnyCwfCy1qdFczT0dmn7h8fpyAdemEavwFeda4d0PApGfSU2jLt39X8kYUBxNM2WgALRBgHdVde87q6Pi5U69TjhMd28W1SFD1DxyogCCrqOct2ZPICoLnrqdF3OdNzjRVLfeyvQ8LgLvRNFR9WfWAyAz79nKgBamd8Ntlvt4Mg35E5gVS2g7AQ7rkm72cBdnW9sCEyGabeXAuH5j4GRbuLT7qBZWDcFLF4SsCdS3WfFGdNHfwaijzykByo71PvFVlTXH2WJWoFvR5FALjBTn7bCdP0pAiSbLCY8Xz2Msc3dBb5Ff9GISPbUpNmUvBdMZMHQvqOmTNXEPpN0b74MDOMQfWJShOo3NkAvMjs") == test.testComplexField().testFieldString());

    test.deserialize(QByteArray::fromHex("0854128404328104595652664a766a78716267764677533159764f5a5867746a356666474c533741694e487a396f5a496f4b626d377a3848373978427579506b70515876476f4f30394f5939785261777833654f417339786a6f544131784a68727732385441637131436562596c43395755665143366849616e74614e647948694b546f666669305a74376c613432535278585a53503447757862635a49703533704a6e79437766437931716446637a5430646d6e3768386670794164656d456176774665646134643050417047665355326a4c74333958386b595542784e4d325767414c524267486456646538377136506935553639546a684d6432385731534644314478796f67434372714f6374325a5049436f4c6e72716446334f644e7a6a52564c6665797651384c674c76524e4652395766574179417a37396e4b6742616d64384e746c7674344d6733354535675653326737415137726b6d37326342646e5739734345794761626558417548356a34475262754c543771425a574463464c463453734364533357664647644e48667761696a7a796b42796f3731507646566c54584832574a576f4676523546414c6a42546e37624364503070416953624c435938587a324d73633364426235466639474953506255704e6d557642644d5a4d485176714f6d544e584550704e306237344d444f4d5166574a53684f6f334e6b41764d6a73"));
    ASSERT_EQ(42, test.testFieldInt());
    ASSERT_TRUE(QString::fromUtf8("YVRfJvjxqbgvFwS1YvOZXgtj5ffGLS7AiNHz9oZIoKbm7z8H79xBuyPkpQXvGoO09OY9xRawx3eOAs9xjoTA1xJhrw28TAcq1CebYlC9WUfQC6hIantaNdyHiKToffi0Zt7la42SRxXZSP4GuxbcZIp53pJnyCwfCy1qdFczT0dmn7h8fpyAdemEavwFeda4d0PApGfSU2jLt39X8kYUBxNM2WgALRBgHdVde87q6Pi5U69TjhMd28W1SFD1DxyogCCrqOct2ZPICoLnrqdF3OdNzjRVLfeyvQ8LgLvRNFR9WfWAyAz79nKgBamd8Ntlvt4Mg35E5gVS2g7AQ7rkm72cBdnW9sCEyGabeXAuH5j4GRbuLT7qBZWDcFLF4SsCdS3WfFGdNHfwaijzykByo71PvFVlTXH2WJWoFvR5FALjBTn7bCdP0pAiSbLCY8Xz2Msc3dBb5Ff9GISPbUpNmUvBdMZMHQvqOmTNXEPpN0b74MDOMQfWJShOo3NkAvMjs") == test.testComplexField().testFieldString());
}

TEST_F(DeserializationTest, RepeatedStringMessageTest)
{
    RepeatedStringMessage test;
    test.deserialize(QByteArray::fromHex("0a04616161610a0562626262620a036363630a066464646464640a056565656565"));
    ASSERT_EQ(5, test.testRepeatedString().count());
    ASSERT_TRUE(test.testRepeatedString() == QStringList({"aaaa","bbbbb","ccc","dddddd","eeeee"}));
}

TEST_F(DeserializationTest, RepeatedBytesMessageTest)
{
    RepeatedBytesMessage test;
    test.deserialize(QByteArray::fromHex("0a060102030405060a04ffffffff0a05eaeaeaeaea0a06010203040506"));
    ASSERT_EQ(4, test.testRepeatedBytes().count());
    ASSERT_TRUE(test.testRepeatedBytes() == QByteArrayList({QByteArray::fromHex("010203040506"),
                                                             QByteArray::fromHex("ffffffff"),
                                                             QByteArray::fromHex("eaeaeaeaea"),
                                                             QByteArray::fromHex("010203040506")}));
    //TODO: Serialization for list types works partially incorrect because of appending of values to existing
    //Need to make decision if deserialize should reset all protobuf properties or not
    RepeatedBytesMessage test2;
    test2.deserialize(QByteArray::fromHex("0a060102030405060a000a05eaeaeaeaea0a06010203040506"));
    ASSERT_EQ(4, test2.testRepeatedBytes().count());
    ASSERT_TRUE(test2.testRepeatedBytes() == QByteArrayList({QByteArray::fromHex("010203040506"),
                                                             QByteArray::fromHex(""),
                                                             QByteArray::fromHex("eaeaeaeaea"),
                                                             QByteArray::fromHex("010203040506")}));
}

TEST_F(DeserializationTest, RepeatedFloatMessageTest)
{
    RepeatedFloatMessage test;
    test.deserialize(QByteArray::fromHex("0a14cdcccc3e9a99993f0000003f3333b33f9a99193f"));
    ASSERT_EQ(5, test.testRepeatedFloat().count());
    ASSERT_TRUE(test.testRepeatedFloat() == FloatList({0.4f, 1.2f, 0.5f, 1.4f, 0.6f}));
}

TEST_F(DeserializationTest, RepeatedDoubleMessageTest)
{
    RepeatedDoubleMessage test;
    test.deserialize(QByteArray::fromHex("0a289a9999999999b93f9a9999999999c93f333333333333d33f9a9999999999d93f000000000000e03f"));
    ASSERT_EQ(5, test.testRepeatedDouble().count());
    ASSERT_TRUE(test.testRepeatedDouble() == DoubleList({0.1, 0.2, 0.3, 0.4, 0.5}));
}

TEST_F(DeserializationTest, RepeatedIntMessageTest)
{
    RepeatedIntMessage test;
    test.deserialize(QByteArray::fromHex("0a0702a00606080a0c"));
    ASSERT_EQ(6, test.testRepeatedInt().count());
    ASSERT_TRUE(test.testRepeatedInt() == IntList({1, 400, 3, 4, 5, 6}));
}
