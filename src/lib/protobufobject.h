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

    inline static unsigned char getTypeByte(int fieldIndex, WireTypes wireType) {
        /*  Header byte
         *  bits    | 7  6  5  4  3 |  2  1  0
         *  -----------------------------------
         *  meaning |  Field index  |   Type
         */
        unsigned char header = (fieldIndex << 3) | wireType;
        return *(char *)&header;
    }

    inline QByteArray serializeValue(const QVariant& propertyValue, int fieldIndex, bool isFixed = false) {
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
        case QMetaType::User:
            type = LengthDelimited;
            result.append(serializeUserType(propertyValue));
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
            result.prepend(getTypeByte(fieldIndex, type));
        }
        return result;
    }

    QByteArray serializeLengthDelimited(const QByteArray& data) {
        //Varint serialize field size and apply result as starting point
        QByteArray result = serializeVarint(static_cast<unsigned int>(data.size()));
        result.append(data);
        return result;
    }

    QByteArray serializeUserType(const QVariant& propertyValue) {
        int userType = propertyValue.userType();
        if (userType == qMetaTypeId<IntList>()) {

        } else if(userType == qMetaTypeId<FloatList>()) {

        } else if(userType == qMetaTypeId<DoubleList>()) {

        } else {
            Q_ASSERT_X(QMetaType::UnknownType == userType, staticMetaObject.className(), "Serialization of unknown user type");
            const void *src = propertyValue.constData();
            //TODO: each time huge objects will make own copies
            //Probably generate fields reflection is better solution
            auto value = std::unique_ptr<ProtobufObjectPrivate>(reinterpret_cast<ProtobufObjectPrivate *>(QMetaType::create(userType, src)));
            return serializeLengthDelimited(value->serializePrivate());
        }
        return QByteArray();
    }

    QByteArray serializeListType(const QVariantList& listValue, int &outFieldIndex)
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
public:
    virtual QByteArray serializePrivate() = 0;
};

template <typename T>
class ProtobufObject : public ProtobufObjectPrivate
{
protected:
    QByteArray serializePrivate() override {
        return serialize();
    }

public:
    explicit ProtobufObject(QObject *parent = nullptr) : ProtobufObjectPrivate(parent) {}

    QByteArray serialize() {
        QByteArray result;
        T* instance = dynamic_cast<T*>(this);
        for (auto field : T::propertyOrdering) {
            int propertyIndex = field.second;
            int fieldIndex = field.first;
            ASSERT_FIELD_NUMBER(fieldIndex);
            QMetaProperty metaProperty = T::staticMetaObject.property(propertyIndex);
            const char* propertyName = metaProperty.name();
            const QVariant& propertyValue = instance->property(propertyName);
            //TODO: flag isFixed looks ugly. Need to define more effective strategy
            //for type detection.
            result.append(serializeValue(propertyValue, fieldIndex, QString(metaProperty.typeName()).contains("Fixed")));
        }

        return result;
    }

    void deserialize(const QByteArray& array) {
        T* instance = dynamic_cast<T*>(this);
        //TODO
    }
};

}
