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
#include <QDebug>
#include <QMetaObject>
#include <QMetaProperty>
#include <QBitArray>

#include <unordered_map>
#include <memory>
#include <type_traits>
#include <qtprotobuftypes.h>

#define ASSERT_FIELD_NUMBER(X) Q_ASSERT_X(X < 128 && X > 0 && X != NotUsedFieldIndex, T::staticMetaObject.className(), "fieldIndex is out of range")

namespace qtprotobuf {

enum WireTypes {
    UnknownWireType = -1,
    Varint = 0,
    Fixed64 = 1,
    LengthDelimited = 2,
    Fixed32 = 5
};

constexpr int NotUsedFieldIndex = -1;

class ProtobufObjectPrivate : public QObject {
protected:
    explicit ProtobufObjectPrivate(QObject *parent = nullptr) : QObject(parent) {}

    /*  Header byte
     *  bits    | 7  6  5  4  3 |  2  1  0
     *  -----------------------------------
     *  meaning |  Field index  |   Type
     */
    inline static unsigned char encodeHeaderByte(int fieldIndex, WireTypes wireType) {

        unsigned char header = (fieldIndex << 3) | wireType;
        return *(char *)&header;
    }

    inline static bool decodeHeaderByte(unsigned char typeByte, int &fieldIndex, WireTypes &wireType) {
        wireType = static_cast<WireTypes>(typeByte & 0x07);
        fieldIndex = typeByte >> 3;
        return fieldIndex < 128 && fieldIndex > 0 && (wireType == Varint
                                                      || wireType == Fixed64
                                                      || wireType == Fixed32
                                                      || wireType == LengthDelimited);
    }

    QByteArray serializeValue(const QVariant& propertyValue, int fieldIndex, bool isFixed = false) {
        QByteArray result;
        WireTypes type = UnknownWireType;
        switch (propertyValue.type()) {
        case QMetaType::Int:
            type = Varint;
            result.append(serializeVarint(propertyValue.toInt()));
            break;
        case QMetaType::Float:
            type = Fixed32;
            result.append(serializeFixed(propertyValue.toFloat()));
            break;
        case QMetaType::Double:
            type = Fixed64;
            result.append(serializeFixed(propertyValue.toDouble()));
            break;
        case QMetaType::QString:
            type = LengthDelimited;
            result.append(serializeLengthDelimited(propertyValue.toString().toUtf8()));
            break;
        case QMetaType::QByteArray:
            type = LengthDelimited;
            result.append(serializeLengthDelimited(propertyValue.toByteArray()));
            break;
        //TODO: explicit QList<TypeName> is more user friendly and performance efficient
        case QMetaType::QVariantList:
            type = LengthDelimited;
            result.append(serializeListType(propertyValue.toList(), fieldIndex));
            break;
        case QMetaType::QStringList:
            type = LengthDelimited;
            result.append(serializeListType(propertyValue.toStringList(), fieldIndex));
            break;
        case QMetaType::QByteArrayList:
            type = LengthDelimited;
            result.append(serializeListType(propertyValue.value<QByteArrayList>(), fieldIndex));
            break;
        case QMetaType::User:
            type = LengthDelimited;
            result.append(serializeUserType(propertyValue, fieldIndex));
            break;
        case QMetaType::UInt:
            type = Fixed32;
            if (isFixed) {
                result.append(serializeFixed(propertyValue.toUInt()));
            } else {
                result.append(serializeVarint(propertyValue.toUInt()));
            }
            break;
        case QMetaType::ULongLong:
            type = Fixed64;
            if (isFixed) {
                result.append(serializeFixed(propertyValue.toULongLong()));
            } else {
                result.append(serializeVarint(propertyValue.toULongLong()));
            }
            break;
        default:
            Q_ASSERT_X(false, staticMetaObject.className(), "Serialization of unknown type is impossible");
        }
        if (fieldIndex != NotUsedFieldIndex
                && type != UnknownWireType) {
            result.prepend(encodeHeaderByte(fieldIndex, type));
        }
        return result;
    }

    QByteArray serializeLengthDelimited(const QByteArray &data) {
        //Varint serialize field size and apply result as starting point
        QByteArray result = serializeVarint(static_cast<unsigned int>(data.size()));
        result.append(data);
        return result;
    }

    QByteArray serializeUserType(const QVariant &propertyValue, int &fieldIndex) {
        int userType = propertyValue.userType();
        if (userType == qMetaTypeId<IntList>()) {
            return serializeListType(propertyValue.value<IntList>(), fieldIndex);
        } else if(userType == qMetaTypeId<FloatList>()) {
            return serializeListType(propertyValue.value<FloatList>(), fieldIndex);
        } else if(userType == qMetaTypeId<DoubleList>()) {
            return serializeListType(propertyValue.value<DoubleList>(), fieldIndex);
        } else {
            const void *src = propertyValue.constData();
            //TODO: each time huge objects will make own copies
            //Probably generate fields reflection is better solution
            auto value = std::unique_ptr<ProtobufObjectPrivate>(reinterpret_cast<ProtobufObjectPrivate *>(QMetaType::create(userType, src)));
            return serializeLengthDelimited(value->serializePrivate());
        }
        Q_ASSERT_X(QMetaType::UnknownType == userType, staticMetaObject.className(), "Serialization of unknown user type");
        return QByteArray();
    }

    template<typename V,
             typename std::enable_if_t<std::is_integral<V>::value
                                       || std::is_floating_point<V>::value, int> = 0>
    QByteArray serializeListType(const QList<V> &listValue, int &outFieldIndex) {
        if (listValue.count() <= 0) {
            outFieldIndex = NotUsedFieldIndex;
            return QByteArray();
        }

        QByteArray serializedList;
        for(auto& value : listValue) {
            serializedList.append(serializeValue(value, NotUsedFieldIndex));
        }
        //If internal field type is not LengthDelimited, exact amount of fields to be specified
        serializedList.prepend(serializeVarint(static_cast<unsigned int>(serializedList.size())));
        return serializedList;
    }

    template<typename V,
             typename std::enable_if_t<std::is_same<V, QString>::value
                                       || std::is_same<V, QByteArray>::value, int> = 0>
    QByteArray serializeListType(const QList<V> &listValue, int &outFieldIndex) {
        if (listValue.count() <= 0) {
            outFieldIndex = NotUsedFieldIndex;
            return QByteArray();
        }

        QByteArray serializedList;
        for(auto& value : listValue) {
            serializedList.append(serializeValue(value, outFieldIndex));
        }

        outFieldIndex = NotUsedFieldIndex;
        return serializedList;
    }

    //TODO: This specialization is deprecated and won't be used in future
    QByteArray serializeListType(const QVariantList &listValue, int &outFieldIndex)
    {
        if (listValue.count() <= 0) {
            outFieldIndex = NotUsedFieldIndex;
            return QByteArray();
        }
        int itemType = listValue.first().type();
        //If internal serialized type is LengthDelimited, need to specify type for each field
        int inFieldIndex = (itemType == QMetaType::QString ||
                      itemType == QMetaType::QByteArray ||
                      itemType == QMetaType::User) ? outFieldIndex : NotUsedFieldIndex;
        QByteArray serializedList;
        for(auto& value : listValue) {
            serializedList.append(serializeValue(value, inFieldIndex));
        }
        //If internal field type is not LengthDelimited, exact amount of fields to be specified
        if(inFieldIndex == NotUsedFieldIndex) {
            serializedList.prepend(serializeVarint(static_cast<unsigned int>(serializedList.size())));
        } else {
            outFieldIndex = NotUsedFieldIndex;
        }
        return serializedList;
    }

    template <typename V,
              typename std::enable_if_t<std::is_floating_point<V>::value
                                        || std::is_same<V, unsigned int>::value
                                        || std::is_same<V, qulonglong>::value, int> = 0>
    QByteArray serializeFixed(V value) {
        //Reserve required amount of bytes
        QByteArray result(sizeof(V), '\0');
        *(V*)(result.data()) = value;
        return result;
    }

    template <typename V,
              typename std::enable_if_t<std::is_signed<V>::value, int> = 0>
    QByteArray serializeVarint(V value) {
        using UV = typename std::make_unsigned<V>::type;
        //Use ZigZag convertion first and apply unsigned variant next
        value = (value << 1) ^ (value >> (sizeof(UV) * 8 - 1));
        UV uValue = *(UV *)&value;
        return serializeVarint(uValue);
    }

    template <typename V,
              typename std::enable_if_t<std::is_unsigned<V>::value, int> = 0>
    QByteArray serializeVarint(V value) {
        QByteArray result;
        //Reserve maximum required amount of bytes
        result.reserve(sizeof(V));
        while (value > 0) {
            //Put first 7 bits to result buffer and mark as not last
            result.append(value & 0x7F | 0x80);
            //Devide values to chunks of 7 bits, move to next chunk
            value >>= 7;
        }

        //Zero case
        if (result.isEmpty()) {
            result.append('\0');
        }

        //Mark last chunk as last
        result.data()[result.size() - 1] &= ~0x80;
        return result;
    }

//####################################################
//               Deserialization
//####################################################
    void deserializeProperty(WireTypes wireType, const QMetaProperty &metaProperty, QByteArray::const_iterator &it)
    {
        QVariant newPropertyValue;
        int type = metaProperty.type();
        switch(type) {
        case QMetaType::UInt:
            if (wireType == Fixed32) {
                newPropertyValue = deserializeFixed<FixedInt32>(it);
            } else {
                newPropertyValue = deserializeVarint<unsigned int>(it);
            }
            break;
        case QMetaType::ULongLong:
            if (wireType == Fixed64) {
                newPropertyValue = deserializeFixed<FixedInt64>(it);
            } else {
                //TODO: deserialize varint
            }
            break;
        case QMetaType::Float:
            newPropertyValue = deserializeFixed<float>(it);
            break;
        case QMetaType::Double:
            newPropertyValue = deserializeFixed<double>(it);
            break;
        case QMetaType::Int:
            newPropertyValue = deserializeVarint<int>(it);
            break;
        case QMetaType::QString:
            newPropertyValue = QString::fromUtf8(deserializeLengthDelimited(it));
            break;
        case QMetaType::QByteArray:
            newPropertyValue = deserializeLengthDelimited(it);
            break;
        case QMetaType::User:
            deserializeUserType(metaProperty.userType(), it, newPropertyValue);
            break;
        case QMetaType::QByteArrayList: {
            QByteArrayList currentValue = metaProperty.read(this).value<QByteArrayList>();
            currentValue.append(deserializeListType<QByteArray>(it));
            metaProperty.write(this, QVariant::fromValue<QByteArrayList>(currentValue));
        }
            return;
        case QMetaType::QStringList: {
            QStringList currentValue = metaProperty.read(this).value<QStringList>();
            currentValue.append(QString::fromUtf8(deserializeListType<QString>(it)));
            metaProperty.write(this, currentValue);
        }
            return;
        default:
            break;
        }
        metaProperty.write(this, newPropertyValue);
    }

    template <typename V,
              typename std::enable_if_t<std::is_floating_point<V>::value
                                        || std::is_same<V, unsigned int>::value
                                        || std::is_same<V, qulonglong>::value, int> = 0>
    QVariant deserializeFixed(QByteArray::const_iterator &it) {
        QVariant newPropertyValue(QVariant::fromValue(*(V*)it));
        it += sizeof(V);
        return newPropertyValue;
    }

    template <typename V,
              typename std::enable_if_t<std::is_unsigned<V>::value, int> = 0>
    QVariant deserializeVarint(QByteArray::const_iterator &it) {
        return QVariant::fromValue(deserializeVarintCommon<V>(it));
    }

    template <typename V,
              typename std::enable_if_t<std::is_signed<V>::value, int> = 0>
    QVariant deserializeVarint(QByteArray::const_iterator &it) {
        using UV = typename std::make_unsigned<V>::type;
        UV unsignedValue = deserializeVarintCommon<UV>(it);
        V value = (unsignedValue >> 1) ^ (-(unsignedValue & 1));

        return QVariant::fromValue(value);
    }

    template <typename V>
    V deserializeVarintCommon(QByteArray::const_iterator &it) {
        V value = 0;
        int k = 0;
        while((*it) & 0x80) {
            value += ((*it) & 0x7f) << (7 * k);
            ++k;
            ++it;
        }
        value += ((*it) & 0x7f) << (7 * k);
        ++it;
        return value;
    }

    QByteArray deserializeLengthDelimited(QByteArray::const_iterator &it) {
        unsigned int length = deserializeVarint<unsigned int>(it).toUInt();
        QByteArray result(it, length);
        it += length;
        return result;
    }

    void deserializeUserType(int userType, QByteArray::const_iterator& it, QVariant &newValue)
    {
        if (userType == qMetaTypeId<IntList>()) {
            newValue = deserializeVarintListType<int>(it);
        } else if(userType == qMetaTypeId<FloatList>()) {
            newValue = deserializeListType<float>(it);
        } else if(userType == qMetaTypeId<DoubleList>()) {
            newValue = deserializeListType<double>(it);
        } else {
            auto value = reinterpret_cast<ProtobufObjectPrivate *>(QMetaType::create(userType));
            value->deserializePrivate(deserializeLengthDelimited(it));
            newValue = QVariant(userType, value);
        }
    }

    template <typename V,
              typename std::enable_if_t<std::is_same<V, QString>::value
                                        || std::is_same<V, QByteArray>::value, int> = 0>
    QByteArray deserializeListType(QByteArray::const_iterator& it) {
        return deserializeLengthDelimited(it);
    }

    template <typename V,
              typename std::enable_if_t<std::is_floating_point<V>::value
                                        || std::is_same<V, unsigned int>::value
                                        || std::is_same<V, qulonglong>::value, int> = 0>
    QVariant deserializeListType(QByteArray::const_iterator& it) {
        QList<V> out;
        unsigned int count = deserializeVarint<unsigned int>(it).toUInt() / sizeof(V);
        for (int i = 0; i < count; i++) {
            QVariant variant = deserializeFixed<V>(it);
            out.append(variant.value<V>());
        }
        return QVariant::fromValue(out);
    }

    template <typename V,
              typename std::enable_if_t<std::is_same<V, int>::value, int> = 0>
    QVariant deserializeVarintListType(QByteArray::const_iterator& it) {
        QList<V> out;
        unsigned int count = deserializeVarint<unsigned int>(it).toUInt();
        QByteArray::const_iterator lastVarint = it + count;
        while(it != lastVarint) {
            QVariant variant = deserializeVarint<V>(it);
            out.append(variant.value<V>());
        }
        return QVariant::fromValue(out);
    }

public:
    virtual QByteArray serializePrivate() = 0;
    virtual void deserializePrivate(const QByteArray &data) = 0;
};

template <typename T>
class ProtobufObject : public ProtobufObjectPrivate
{
protected:
    QByteArray serializePrivate() override {
        return serialize();
    }

    void deserializePrivate(const QByteArray &data) override {
        deserialize(data);
    }

public:
    explicit ProtobufObject(QObject *parent = nullptr) : ProtobufObjectPrivate(parent) {}

    QByteArray serialize() {
        QByteArray result;
        T *instance = dynamic_cast<T *>(this);
        for (auto field : T::propertyOrdering) {
            int propertyIndex = field.second;
            int fieldIndex = field.first;
            ASSERT_FIELD_NUMBER(fieldIndex);
            QMetaProperty metaProperty = T::staticMetaObject.property(propertyIndex);
            const char *propertyName = metaProperty.name();
            const QVariant &propertyValue = instance->property(propertyName);
            //TODO: flag isFixed looks ugly. Need to define more effective strategy
            //for type detection.
            result.append(serializeValue(propertyValue, fieldIndex, QString(metaProperty.typeName()).contains("Fixed")));
        }

        return result;
    }

    void deserialize(const QByteArray &array) {
        T *instance = dynamic_cast<T *>(this);

        for(QByteArray::const_iterator it = array.begin(); it != array.end();) {
            //Each iteration we expect iterator is setup to beginning of next chunk
            int fieldNumber = NotUsedFieldIndex;
            WireTypes wireType = UnknownWireType;
            if (!decodeHeaderByte(*it, fieldNumber, wireType)) {
                ++it;
                qCritical() << "Message received doesn't contains valid header byte. "
                               "Trying next, but seems stream is broken";
                continue;
            }

            auto propertyNumberIt = T::propertyOrdering.find(fieldNumber);
            if (propertyNumberIt == std::end(T::propertyOrdering)) {
                ++it;
                qCritical() << "Message received contains invalid field number. "
                               "Trying next, but seems stream is broken";
                continue;
            }

            int propertyIndex = propertyNumberIt->second;
            QMetaProperty metaProperty = T::staticMetaObject.property(propertyIndex);

            ++it;
            deserializeProperty(wireType, metaProperty, it);
        }
    }
};

}
