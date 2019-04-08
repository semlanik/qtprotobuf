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

#pragma once

#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QBitArray>

#include <unordered_map>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <functional>

#include <qtprotobuftypes.h>
#include <qtprotobuflogging.h>

#define ASSERT_FIELD_NUMBER(X) Q_ASSERT_X(X < 128 && X > 0 && X != NotUsedFieldIndex, T::staticMetaObject.className(), "fieldIndex is out of range")

namespace qtprotobuf {

enum WireTypes {
    UnknownWireType = -1,
    Varint = 0,
    Fixed64 = 1,
    LengthDelimited = 2,
    Fixed32 = 5
};

template<typename V>
struct make_unsigned { typedef typename std::make_unsigned<V>::type type; };

template<>
struct make_unsigned<sint32> { typedef typename std::make_unsigned<decltype(sint32::_t)>::type type; };

template<>
struct make_unsigned<sint64> { typedef typename std::make_unsigned<decltype(sint64::_t)>::type type; };

constexpr int NotUsedFieldIndex = -1;

class ProtobufObjectPrivate : public QObject
{
protected:
    using Serializer = std::function<QByteArray(const QVariant &, int &)>;
    using Deserializer = std::function<void(ProtobufObjectPrivate *, QByteArray::const_iterator &, QVariant &)>;
    struct SerializationHandlers {
        Serializer serializer;
        Deserializer deserializer;
    };

    using SerializerRegistry = std::unordered_map<int/*metatypeid*/, SerializationHandlers>;
    static SerializerRegistry serializers;

public:
    explicit ProtobufObjectPrivate(QObject *parent = nullptr) : QObject(parent) {}

    inline static unsigned char encodeHeaderByte(int fieldIndex, WireTypes wireType);
    inline static bool decodeHeaderByte(unsigned char typeByte, int &fieldIndex, WireTypes &wireType);

    static QByteArray serializeValue(const QVariant &propertyValue, int fieldIndex, const QMetaProperty &metaProperty);
    static QByteArray serializeUserType(const QVariant &propertyValue, int &fieldIndex, const QLatin1Literal &typeName, WireTypes &type);

    void deserializeProperty(WireTypes wireType, const QMetaProperty &metaProperty, QByteArray::const_iterator &it);
    void deserializeUserType(const QMetaProperty &metaType, QByteArray::const_iterator &it, QVariant &newValue);

    //###########################################################################
    //                           Serialization helpers
    //###########################################################################
    static QByteArray serializeLengthDelimited(const QString &data) {
        qProtoDebug() << __func__ << "data.size" << data.size() << "data" << data;
        return serializeLengthDelimited(data.toUtf8());
    }

    static QByteArray serializeLengthDelimited(const QByteArray &data) {
        qProtoDebug() << __func__ << "data.size" << data.size() << "data" << data.toHex();

        //Varint serialize field size and apply result as starting point
        QByteArray result = serializeVarintZero(static_cast<unsigned int>(data.size()));
        result.append(data);
        return result;
    }

    template<typename V,
             typename std::enable_if_t<std::is_same<sint32, V>::value
                                       || std::is_same<sint64, V>::value, int> = 0>
    static QByteArray serializeListTypeZigZag(const QList<V> &listValue, int &outFieldIndex) {
        qProtoDebug() << __func__ << "listValue.count" << listValue.count() << "outFiledIndex" << outFieldIndex;

        if (listValue.count() <= 0) {
            outFieldIndex = NotUsedFieldIndex;
            return QByteArray();
        }

        QByteArray serializedList;
        for (auto &value : listValue) {
            serializedList.append(serializeVarintZigZag(value));
        }
        //If internal field type is not LengthDelimited, exact amount of fields to be specified
        serializedList.prepend(serializeVarintZero(static_cast<unsigned int>(serializedList.size())));
        return serializedList;
    }

    template<typename V,
             typename std::enable_if_t<std::is_same<V, int32>::value
                                       || std::is_integral<V>::value, int> = 0>
    static QByteArray serializeListType(const QList<V> &listValue, int &outFieldIndex) {
        qProtoDebug() << __func__ << "listValue.count" << listValue.count() << "outFiledIndex" << outFieldIndex;

        if (listValue.count() <= 0) {
            outFieldIndex = NotUsedFieldIndex;
            return QByteArray();
        }

        QByteArray serializedList;
        for (auto &value : listValue) {
            serializedList.append(serializeVarint<V>(value));
        }
        //If internal field type is not LengthDelimited, exact amount of fields to be specified
        serializedList.prepend(serializeVarintZero(static_cast<unsigned int>(serializedList.size())));
        return serializedList;
    }

    template<typename V,
             typename std::enable_if_t<std::is_floating_point<V>::value
                                       || std::is_same<V, fint32>::value
                                       || std::is_same<V, fint64>::value
                                       || std::is_same<V, sfint32>::value
                                       || std::is_same<V, sfint64>::value, int> = 0>
    static QByteArray serializeListType(const QList<V> &listValue, int &outFieldIndex) {
        qProtoDebug() << __func__ << "listValue.count" << listValue.count() << "outFiledIndex" << outFieldIndex;

        if (listValue.count() <= 0) {
            outFieldIndex = NotUsedFieldIndex;
            return QByteArray();
        }

        QByteArray serializedList;
        for (auto &value : listValue) {
            serializedList.append(serializeFixed(value));
        }
        //If internal field type is not LengthDelimited, exact amount of fields to be specified
        serializedList.prepend(serializeVarintZero(static_cast<unsigned int>(serializedList.size())));
        return serializedList;
    }

    template<typename V,
             typename std::enable_if_t<std::is_same<V, QString>::value
                                       || std::is_same<V, QByteArray>::value, int> = 0>
    static QByteArray serializeListType(const QList<V> &listValue, int &outFieldIndex) {
        qProtoDebug() << __func__ << "listValue.count" << listValue.count() << "outFiledIndex" << outFieldIndex;

        if (listValue.count() <= 0) {
            outFieldIndex = NotUsedFieldIndex;
            return QByteArray();
        }

        QByteArray serializedList;
        for (auto &value : listValue) {
            serializedList.append(encodeHeaderByte(outFieldIndex, LengthDelimited));
            serializedList.append(serializeLengthDelimited(value));
        }

        outFieldIndex = NotUsedFieldIndex;
        return serializedList;
    }

    template<typename V,
             typename std::enable_if_t<std::is_base_of<ProtobufObjectPrivate, V>::value, int> = 0>
    static QByteArray serializeListType(const QList<V> &listValue, int &outFieldIndex) {
        qProtoDebug() << __func__ << "listValue.count" << listValue.count() << "outFiledIndex" << outFieldIndex;

        if (listValue.count() <= 0) {
            outFieldIndex = NotUsedFieldIndex;
            return QByteArray();
        }

        QByteArray serializedList;
        for (auto &value : listValue) {
            QByteArray serializedValue = serializeLengthDelimited(value.serialize());
            serializedValue.prepend(encodeHeaderByte(outFieldIndex, LengthDelimited));
            serializedList.append(serializedValue);
        }

        outFieldIndex = NotUsedFieldIndex;

        return serializedList;
    }

    template <typename V,
              typename std::enable_if_t<std::is_floating_point<V>::value
                                        || std::is_same<V, fint32>::value
                                        || std::is_same<V, fint64>::value
                                        || std::is_same<V, sfint32>::value
                                        || std::is_same<V, sfint64>::value, int> = 0>
    static QByteArray serializeFixed(V value) {
        qProtoDebug() << __func__ << "value" << value;

        //Reserve required amount of bytes
        QByteArray result(sizeof(V), '\0');
        *(V*)(result.data()) = value;
        return result;
    }

    template <typename V, typename UV = typename qtprotobuf::make_unsigned<V>::type,
              typename std::enable_if_t<std::is_signed<V>::value, int> = 0>
    static QByteArray serializeVarint(V value) {
        qProtoDebug() << __func__ << "value" << value;

        return serializeVarint(static_cast<UV>(value));
    }

    template <typename V, typename UV = typename qtprotobuf::make_unsigned<V>::type,
              typename std::enable_if_t<std::is_same<V, sint32>::value
                                        || std::is_same<V, sint64>::value, int> = 0>
    static QByteArray serializeVarintZigZag(V value) {
        qProtoDebug() << __func__ << "value" << value;

        UV uValue = 0;

        //Use ZigZag convertion first and apply unsigned variant next
        value = (value << 1) ^ (value >> (sizeof(UV) * 8 - 1));
        uValue = static_cast<UV>(value);
        return serializeVarint(uValue);
    }

    template <typename V,
              typename std::enable_if_t<std::is_unsigned<V>::value, int> = 0>
    static QByteArray serializeVarint(V value) {
        qProtoDebug() << __func__ << "value" << value;

        QByteArray result;
        //Reserve maximum required amount of bytes
        result.reserve(sizeof(V));
        while (value > 0) {
            //Put first 7 bits to result buffer and mark as not last
            result.append((value & 0x7F) | 0x80);
            //Devide values to chunks of 7 bits, move to next chunk
            value >>= 7;
        }

        //TODO: Zero case.
        //Aligned to reference cpp implementation. Where 0 ignored.
        //if (result.isEmpty()) {
        //    result.append('\0');
        //}

        //Mark last chunk as last
        result.data()[result.size() - 1] &= ~0x80;
        return result;
    }

    template <typename V,
              typename std::enable_if_t<std::is_unsigned<V>::value, int> = 0>
    static QByteArray serializeVarintZero(V value) {
        qProtoDebug() << __func__ << "value" << value;

        QByteArray result = serializeVarint(value);
        //Zero case.
        if (result.isEmpty()) {
            result.append('\0');
        }
        return result;
    }

    //###########################################################################
    //                           Deserialization helpers
    //###########################################################################
    template <typename V,
              typename std::enable_if_t<std::is_floating_point<V>::value
                                        || std::is_same<V, fint32>::value
                                        || std::is_same<V, fint64>::value
                                        || std::is_same<V, sfint32>::value
                                        || std::is_same<V, sfint64>::value, int> = 0>
    QVariant deserializeFixed(QByteArray::const_iterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        QVariant newPropertyValue(QVariant::fromValue(*(V*)it));
        it += sizeof(V);
        return newPropertyValue;
    }

    template <typename V,
              typename std::enable_if_t<std::is_unsigned<V>::value, int> = 0>
    QVariant deserializeVarint(QByteArray::const_iterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        return QVariant::fromValue(deserializeVarintCommon<V>(it));
    }

    template <typename V, typename UV = typename qtprotobuf::make_unsigned<V>::type,
              typename std::enable_if_t<std::is_same<sint32, V>::value
                                        || std::is_same<sint64, V>::value,int> = 0>
    QVariant deserializeVarintZigZag(QByteArray::const_iterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        UV unsignedValue = deserializeVarintCommon<UV>(it);
        V value = (unsignedValue >> 1) ^ (-(unsignedValue & 1));
        return QVariant::fromValue<V>(value);
    }

    template <typename V, typename UV = typename qtprotobuf::make_unsigned<V>::type,
              typename std::enable_if_t<std::is_signed<V>::value, int> = 0>
    QVariant deserializeVarint(QByteArray::const_iterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        UV unsignedValue = deserializeVarintCommon<UV>(it);
        V value = static_cast<V>(unsignedValue);
        return QVariant::fromValue(value);
    }

    template <typename V>
    V deserializeVarintCommon(QByteArray::const_iterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        V value = 0;
        int k = 0;
        while (true) {
            uint64_t byte = static_cast<uint64_t>(*it);
            value += (byte & 0x7f) << k;
            k += 7;
            if (((*it) & 0x80) == 0) {
                break;
            }
            ++it;
        }
        ++it;
        return value;
    }

    QByteArray deserializeLengthDelimited(QByteArray::const_iterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        unsigned int length = deserializeVarint<unsigned int>(it).toUInt();
        QByteArray result(it, length);
        it += length;
        return result;
    }

    template <typename V,
              typename std::enable_if_t<std::is_same<V, QString>::value
                                        || std::is_same<V, QByteArray>::value, int> = 0>
    QByteArray deserializeListType(QByteArray::const_iterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        return deserializeLengthDelimited(it);
    }

    template <typename V,
              typename std::enable_if_t<std::is_base_of<ProtobufObjectPrivate, V>::value, int> = 0>
    QVariant deserializeListType(QByteArray::const_iterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);
        QVariant newValue;
        serializers[qMetaTypeId<V>()].deserializer(this, it, newValue);
        return newValue;
    }

    template <typename V,
              typename std::enable_if_t<std::is_floating_point<V>::value
                                        || std::is_same<V, sfint32>::value
                                        || std::is_same<V, sfint64>::value
                                        || std::is_same<V, fint32>::value
                                        || std::is_same<V, fint64>::value, int> = 0>
    QVariant deserializeListType(QByteArray::const_iterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        QList<V> out;
        unsigned int count = deserializeVarint<unsigned int>(it).toUInt() / sizeof(V);
        for (unsigned int i = 0; i < count; i++) {
            QVariant variant = deserializeFixed<V>(it);
            out.append(variant.value<V>());
        }
        return QVariant::fromValue(out);
    }

    template <typename V>
    QVariant deserializeVarintListType(QByteArray::const_iterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        QList<V> out;
        unsigned int count = deserializeVarint<unsigned int>(it).toUInt();
        QByteArray::const_iterator lastVarint = it + count;
        while (it != lastVarint) {
            QVariant variant = deserializeVarint<V>(it);
            out.append(variant.value<V>());
        }
        return QVariant::fromValue(out);
    }

    template <typename V>
    QVariant deserializeVarintListTypeZigZag(QByteArray::const_iterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        QList<V> out;
        unsigned int count = deserializeVarint<unsigned int>(it).toUInt();
        QByteArray::const_iterator lastVarint = it + count;
        while (it != lastVarint) {
            QVariant variant = deserializeVarintZigZag<V>(it);
            out.append(variant.value<V>());
        }
        return QVariant::fromValue(out);
    }
};

/*  Header byte
 *  bits    | 7  6  5  4  3 | 2  1  0
 *  -----------------------------------
 *  meaning |  Field index  |  Type
 */
unsigned char ProtobufObjectPrivate::encodeHeaderByte(int fieldIndex, WireTypes wireType)
{
    unsigned char header = (fieldIndex << 3) | wireType;
    return static_cast<char>(header);
}

bool ProtobufObjectPrivate::decodeHeaderByte(unsigned char typeByte, int &fieldIndex, WireTypes &wireType)
{
    wireType = static_cast<WireTypes>(typeByte & 0x07);
    fieldIndex = typeByte >> 3;
    return fieldIndex < 128 && fieldIndex > 0 && (wireType == Varint
                                                  || wireType == Fixed64
                                                  || wireType == Fixed32
                                                  || wireType == LengthDelimited);
}

}
