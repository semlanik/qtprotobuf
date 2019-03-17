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

constexpr int NotUsedFieldIndex = -1;

class ProtobufObjectPrivate : public QObject
{
protected:
    using ListSerializer = std::function<QByteArray(const ProtobufObjectPrivate *, const QVariant &, int &)>;
    using ListDeserializer = std::function<void(ProtobufObjectPrivate *, QByteArray::const_iterator &, QVariant &)>;
    struct SerializationHandlers {
        ListSerializer serializer;
        ListDeserializer deserializer;
    };

    using SerializerRegistry = std::unordered_map<int/*metatypeid*/, SerializationHandlers>;
    static SerializerRegistry serializers;

public:
    explicit ProtobufObjectPrivate(QObject *parent = nullptr) : QObject(parent) {}

    virtual QByteArray serializePrivate() const = 0;
    virtual void deserializePrivate(const QByteArray &data) = 0;

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

    QByteArray serializeValue(const QVariant& propertyValue, int fieldIndex, const QLatin1Literal& typeName) const {
        qProtoDebug() << __func__ << "propertyValue" << propertyValue << "fieldIndex" << fieldIndex << "typeName" << typeName;
        QByteArray result;
        WireTypes type = UnknownWireType;
        int resultSize = result.size();
        switch (static_cast<QMetaType::Type>(propertyValue.type())) {
        case QMetaType::Int:
            type = Varint;
            if (typeName == "qtprotobuf::sint32"
                    || typeName == "sint32") {
                result.append(serializeVarintZigZag(propertyValue.toInt()));
            } else if (typeName == "qtprotobuf::sfint32"
                       || typeName == "sfint32") {
                type = Fixed32;
                result.append(serializeFixed(propertyValue.toInt()));
            } else {
                result.append(serializeVarint(propertyValue.toLongLong()));
            }
            if (resultSize == result.size()) {
                fieldIndex = NotUsedFieldIndex;
            }
            break;
        case QMetaType::LongLong:
            type = Varint;
            if (typeName == "qtprotobuf::sint64"
                    || typeName == "sint64") {
                result.append(serializeVarintZigZag(propertyValue.toLongLong()));
            } else if (typeName == "qtprotobuf::sfint64"
                       || typeName == "sfint64") {
                type = Fixed64;
                result.append(serializeFixed(propertyValue.toLongLong()));
            } else {
                result.append(serializeVarint(propertyValue.toLongLong()));
            }
            if (resultSize == result.size()) {
                fieldIndex = NotUsedFieldIndex;
            }
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
            if (typeName == "qtprotobuf::fint32"
                    || typeName == "fint32") {
                type = Fixed32;
                result.append(serializeFixed(propertyValue.toUInt()));
            } else {
                type = Varint;
                result.append(serializeVarint(propertyValue.toUInt()));
            }
            if (resultSize == result.size()) {
                fieldIndex = NotUsedFieldIndex;
            }
            break;
        case QMetaType::ULongLong:
            if (typeName == "qtprotobuf::fint64"
                    || typeName == "fint64") {
                type = Fixed64;
                result.append(serializeFixed(propertyValue.toULongLong()));
            } else {
                type = Varint;
                result.append(serializeVarint(propertyValue.toULongLong()));
            }
            if (resultSize == result.size()) {
                fieldIndex = NotUsedFieldIndex;
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

    QByteArray serializeLengthDelimited(const QByteArray &data) const {
        qProtoDebug() << __func__ << "data.size" << data.size() << "data" << data.toHex();
        //Varint serialize field size and apply result as starting point
        QByteArray result = serializeVarintZero(static_cast<unsigned int>(data.size()));
        result.append(data);
        return result;
    }

    QByteArray serializeUserType(const QVariant &propertyValue, int &fieldIndex) const {
        qProtoDebug() << __func__ << "propertyValue" << propertyValue << "fieldIndex" << fieldIndex;
        int userType = propertyValue.userType();

        //First looking type serializer in registred serializers
        auto it = serializers.find(userType);
        if (it != std::end(serializers)) {
            return (it->second).serializer(this, propertyValue, fieldIndex);
        }

        //Check if it's special list
        if (userType == qMetaTypeId<int32List>()) {
            return serializeListType(propertyValue.value<int32List>(), fieldIndex);
        }

        if(userType == qMetaTypeId<FloatList>()) {
            return serializeListType(propertyValue.value<FloatList>(), fieldIndex);
        }

        if(userType == qMetaTypeId<DoubleList>()) {
            return serializeListType(propertyValue.value<DoubleList>(), fieldIndex);
        }

        //Otherwise it's user type
        const void *src = propertyValue.constData();
        //TODO: each time huge objects will make own copies
        //Probably generate fields reflection is better solution
        auto value = std::unique_ptr<ProtobufObjectPrivate>(reinterpret_cast<ProtobufObjectPrivate *>(QMetaType::create(userType, src)));
        return serializeLengthDelimited(value->serializePrivate());
    }

    template<typename V,
             typename std::enable_if_t<std::is_integral<V>::value, int> = 0>
    QByteArray serializeListType(const QList<V> &listValue, int &outFieldIndex) const {
        qProtoDebug() << __func__ << "listValue.count" << listValue.count() << "outFiledIndex" << outFieldIndex;
        if (listValue.count() <= 0) {
            outFieldIndex = NotUsedFieldIndex;
            return QByteArray();
        }

        QByteArray serializedList;
        for(auto& value : listValue) {
            serializedList.append(serializeVarintZigZag(value));
        }
        //If internal field type is not LengthDelimited, exact amount of fields to be specified
        serializedList.prepend(serializeVarintZero(static_cast<unsigned int>(serializedList.size())));
        return serializedList;
    }

    template<typename V,
             typename std::enable_if_t<std::is_floating_point<V>::value, int> = 0>
    QByteArray serializeListType(const QList<V> &listValue, int &outFieldIndex) const {
        qProtoDebug() << __func__ << "listValue.count" << listValue.count() << "outFiledIndex" << outFieldIndex;
        if (listValue.count() <= 0) {
            outFieldIndex = NotUsedFieldIndex;
            return QByteArray();
        }

        QByteArray serializedList;
        for(auto& value : listValue) {
            serializedList.append(serializeFixed(value));
        }
        //If internal field type is not LengthDelimited, exact amount of fields to be specified
        serializedList.prepend(serializeVarintZero(static_cast<unsigned int>(serializedList.size())));
        return serializedList;
    }

    template<typename V,
             typename std::enable_if_t<std::is_same<V, QString>::value
                                       || std::is_same<V, QByteArray>::value, int> = 0>
    QByteArray serializeListType(const QList<V> &listValue, int &outFieldIndex) const {
        qProtoDebug() << __func__ << "listValue.count" << listValue.count() << "outFiledIndex" << outFieldIndex;
        if (listValue.count() <= 0) {
            outFieldIndex = NotUsedFieldIndex;
            return QByteArray();
        }

        QByteArray serializedList;
        for(auto& value : listValue) {
            serializedList.append(serializeValue(value, outFieldIndex, QLatin1Literal()));
        }

        outFieldIndex = NotUsedFieldIndex;
        return serializedList;
    }

    template<typename V,
             typename std::enable_if_t<std::is_base_of<ProtobufObjectPrivate, V>::value, int> = 0>
    QByteArray serializeListType(const QList<V> &listValue, int &outFieldIndex) const {
        qProtoDebug() << __func__ << "listValue.count" << listValue.count() << "outFiledIndex" << outFieldIndex;
        if (listValue.count() <= 0) {
            outFieldIndex = NotUsedFieldIndex;
            return QByteArray();
        }

        QByteArray serializedList;
        for(auto& value : listValue) {
            QByteArray serializedValue = serializeLengthDelimited(value.serialize());
            serializedValue.prepend(encodeHeaderByte(outFieldIndex, LengthDelimited));
            serializedList.append(serializedValue);
        }

        outFieldIndex = NotUsedFieldIndex;

        return serializedList;
    }

    template <typename V,
              typename std::enable_if_t<std::is_floating_point<V>::value
                                        || std::is_same<V, unsigned int>::value
                                        || std::is_same<V, qulonglong>::value
                                        || std::is_same<V, int>::value
                                        || std::is_same<V, qlonglong>::value, int> = 0>
    QByteArray serializeFixed(V value) const {
        qProtoDebug() << __func__ << "value" << value;
        //Reserve required amount of bytes
        QByteArray result(sizeof(V), '\0');
        *(V*)(result.data()) = value;
        return result;
    }

    template <typename V, typename UV = typename std::make_unsigned<V>::type,
              typename std::enable_if_t<std::is_signed<V>::value, int> = 0>
    QByteArray serializeVarint(V value) const {
        qProtoDebug() << __func__ << "value" << value;
        return serializeVarint(static_cast<UV>(value));
    }

    template <typename V, typename UV = typename std::make_unsigned<V>::type,
              typename std::enable_if_t<std::is_signed<V>::value, int> = 0>
    QByteArray serializeVarintZigZag(V value) const {
        qProtoDebug() << __func__ << "value" << value;
        UV uValue = 0;

        //Use ZigZag convertion first and apply unsigned variant next
        value = (value << 1) ^ (value >> (sizeof(UV) * 8 - 1));
        uValue = *(UV *)&value;
        return serializeVarint(uValue);
    }

    template <typename V,
              typename std::enable_if_t<std::is_unsigned<V>::value, int> = 0>
    QByteArray serializeVarint(V value) const {
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
    QByteArray serializeVarintZero(V value) const {
        qProtoDebug() << __func__ << "value" << value;
        QByteArray result = serializeVarint(value);
        //Zero case.
        if (result.isEmpty()) {
            result.append('\0');
        }
        return result;
    }

//####################################################
//               Deserialization
//####################################################
    void deserializeProperty(WireTypes wireType, const QMetaProperty &metaProperty, QByteArray::const_iterator &it)
    {
        QLatin1Literal typeName(metaProperty.typeName());
        qProtoDebug() << __func__ << " wireType: " << wireType << " metaProperty: " << typeName << "currentByte:" << QString::number((*it), 16);
        QVariant newPropertyValue;
        int type = metaProperty.type();
        switch(type) {
        case QMetaType::UInt:
            if (wireType == Fixed32) {
                newPropertyValue = deserializeFixed<fint32>(it);
            } else {
                newPropertyValue = deserializeVarint<uint>(it);
            }
            break;
        case QMetaType::ULongLong:
            if (wireType == Fixed64) {
                newPropertyValue = deserializeFixed<fint64>(it);
            } else {
                newPropertyValue = deserializeVarint<uint64>(it);
            }
            break;
        case QMetaType::Float:
            newPropertyValue = deserializeFixed<float>(it);
            break;
        case QMetaType::Double:
            newPropertyValue = deserializeFixed<double>(it);
            break;
        case QMetaType::Int:
            if (typeName == "sint32"
                    || typeName == "qtprotobuf::sint32") {
                newPropertyValue = deserializeVarintZigZag<sint32>(it);
            } else {
                newPropertyValue = deserializeVarint<int64>(it);
            }
            break;
        case QMetaType::LongLong:
            if (typeName == "sint64"
                    || typeName == "qtprotobuf::sint64") {
                newPropertyValue = deserializeVarintZigZag<sint64>(it);
            } else {
                newPropertyValue = deserializeVarint<int64>(it);
            }
            break;
        case QMetaType::QString:
            newPropertyValue = QString::fromUtf8(deserializeLengthDelimited(it));
            break;
        case QMetaType::QByteArray:
            newPropertyValue = deserializeLengthDelimited(it);
            break;
        case QMetaType::User:
            newPropertyValue = metaProperty.read(this);
            deserializeUserType(metaProperty, it, newPropertyValue);
            break;
        case QMetaType::QByteArrayList: {
            QByteArrayList currentValue = metaProperty.read(this).value<QByteArrayList>();
            currentValue.append(deserializeLengthDelimited(it));
            metaProperty.write(this, QVariant::fromValue<QByteArrayList>(currentValue));
        }
            return;
        case QMetaType::QStringList: {
            QStringList currentValue = metaProperty.read(this).value<QStringList>();
            currentValue.append(QString::fromUtf8(deserializeLengthDelimited(it)));
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

    template <typename V, typename UV = typename std::make_unsigned<V>::type,
              typename std::enable_if_t<std::is_signed<V>::value, int> = 0>
    QVariant deserializeVarintZigZag(QByteArray::const_iterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);
        UV unsignedValue = deserializeVarintCommon<UV>(it);
        V value = (unsignedValue >> 1) ^ (-(unsignedValue & 1));
        return QVariant::fromValue(value);
    }

    template <typename V, typename UV = typename std::make_unsigned<V>::type,
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
        while(true) {
            uint64_t byte = static_cast<uint64_t>(*it);
            value += (byte & 0x7f) << k;
            k += 7;
            if(((*it) & 0x80) == 0) {
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

    void deserializeUserType(const QMetaProperty &metaType, QByteArray::const_iterator& it, QVariant &newValue)
    {
        int userType = metaType.userType();
        QLatin1Literal typeName(metaType.typeName());
        qProtoDebug() << __func__ << "userType" << userType << "currentByte:" << QString::number((*it), 16);

        auto serializerIt = serializers.find(userType);
        if (serializerIt != std::end(serializers)) {
            (serializerIt->second).deserializer(this, it, newValue);
            return;
        }

        if (userType == qMetaTypeId<int32List>()) {
            if (typeName == "sint32List"
                    || typeName == "qtprotobuf::sint32List") {
                newValue = deserializeVarintListTypeZigZag<int32>(it);
            } else {
                newValue = deserializeVarintListType<int32>(it);
            }
        } else if (userType == qMetaTypeId<int64List>()) {
            if (typeName == "sint64List"
                    || typeName == "qtprotobuf::sint64List") {
                newValue = deserializeVarintListTypeZigZag<int64>(it);
            } else {
                newValue = deserializeVarintListType<int64>(it);
            }
        } else if (userType == qMetaTypeId<uint32List>()) {
            //TODO: Check if type is fixed
            newValue = deserializeVarintListType<uint32>(it);
        } else if (userType == qMetaTypeId<uint64List>()) {
            //TODO: Check if type is fixed
            newValue = deserializeVarintListType<uint64>(it);
        } else if(userType == qMetaTypeId<FloatList>()) {
            newValue = deserializeListType<float>(it);
        } else if(userType == qMetaTypeId<DoubleList>()) {
            newValue = deserializeListType<double>(it);
        } else {
            newValue = deserializeProtobufObjectType(userType, it);
        }
    }

    QVariant deserializeProtobufObjectType(int userType, QByteArray::const_iterator &it) {
        auto value = reinterpret_cast<ProtobufObjectPrivate *>(QMetaType::create(userType));
        value->deserializePrivate(deserializeLengthDelimited(it));
        return QVariant(userType, value);
    }

    template <typename V,
              typename std::enable_if_t<std::is_same<V, QString>::value
                                        || std::is_same<V, QByteArray>::value, int> = 0>
    QByteArray deserializeListType(QByteArray::const_iterator& it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);
        return deserializeLengthDelimited(it);
    }

    template <typename V,
              typename std::enable_if_t<std::is_base_of<ProtobufObjectPrivate, V>::value, int> = 0>
    QVariant deserializeListType(QByteArray::const_iterator& it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);
        return deserializeProtobufObjectType(qMetaTypeId<V>(), it);
    }

    template <typename V,
              typename std::enable_if_t<std::is_floating_point<V>::value
                                        || std::is_same<V, unsigned int>::value
                                        || std::is_same<V, qulonglong>::value, int> = 0>
    QVariant deserializeListType(QByteArray::const_iterator& it) {
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
    QVariant deserializeVarintListType(QByteArray::const_iterator& it) {
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
    QVariant deserializeVarintListTypeZigZag(QByteArray::const_iterator& it) {
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

template <typename T>
class ProtobufObject : public ProtobufObjectPrivate
{
public:
    explicit ProtobufObject(QObject *parent = nullptr) : ProtobufObjectPrivate(parent) {}

    QByteArray serialize() const {
        qProtoDebug() << T::staticMetaObject.className() << "serialize";
        QByteArray result;
        const T *instance = dynamic_cast<const T *>(this);
        for (auto field : T::propertyOrdering) {
            int propertyIndex = field.second;
            int fieldIndex = field.first;
            ASSERT_FIELD_NUMBER(fieldIndex);
            QMetaProperty metaProperty = T::staticMetaObject.property(propertyIndex);
            const char *propertyName = metaProperty.name();
            const QVariant &propertyValue = instance->property(propertyName);
            //TODO: flag isFixed looks ugly. Need to define more effective strategy
            //for type detection.
            result.append(serializeValue(propertyValue, fieldIndex, QLatin1Literal(metaProperty.typeName())));
        }

        return result;
    }

    void deserialize(const QByteArray &array) {
        qProtoDebug() << T::staticMetaObject.className() << "deserialize";
        //T *instance = dynamic_cast<T *>(this);

        for (QByteArray::const_iterator it = array.begin(); it != array.end();) {
            //Each iteration we expect iterator is setup to beginning of next chunk
            int fieldNumber = NotUsedFieldIndex;
            WireTypes wireType = UnknownWireType;
            if (!decodeHeaderByte(*it, fieldNumber, wireType)) {
                ++it;
                qProtoCritical() << "Message received doesn't contains valid header byte. "
                               "Trying next, but seems stream is broken";
                continue;
            }

            auto propertyNumberIt = T::propertyOrdering.find(fieldNumber);
            if (propertyNumberIt == std::end(T::propertyOrdering)) {
                ++it;
                qProtoCritical() << "Message received contains invalid field number. "
                               "Trying next, but seems stream is broken";
                continue;
            }

            int propertyIndex = propertyNumberIt->second;
            QMetaProperty metaProperty = T::staticMetaObject.property(propertyIndex);

            ++it;
            deserializeProperty(wireType, metaProperty, it);
        }
    }

protected:
    static void registerSerializers(int metaTypeId, int listMetaTypeId) {
        serializers[listMetaTypeId] = {ListSerializer(serializeComplexListType), ListDeserializer(deserializeComplexListType)};
    }

private:
    QByteArray serializePrivate() const override {
        qProtoDebug() << T::staticMetaObject.className() << "serializePrivate";
        return serialize();
    }

    void deserializePrivate(const QByteArray &data) override {
        qProtoDebug() << T::staticMetaObject.className() << "deserializePrivate";
        deserialize(data);
    }

//TODO: migrate to this function for complex types serialization
//    static QByteArray serializeSelf(const QVariant &variantValue) {
//        T value = variantValue.value<T>();
//        return value->serialize();
//    }

    static QByteArray serializeComplexListType(const ProtobufObjectPrivate* serializer, const QVariant &listValue, int &outFieldIndex) {
        QList<T> list = listValue.value<QList<T>>();
        return serializer->serializeListType(list, outFieldIndex);
    }

    static void deserializeComplexListType(ProtobufObjectPrivate* deserializer, QByteArray::const_iterator &it, QVariant &previous) {
        QList<T> previousList = previous.value<QList<T>>();
        QVariant newMember = deserializer->deserializeListType<T>(it);
        previousList.append(newMember.value<T>());
        previous.setValue(previousList);
    }
};

}
