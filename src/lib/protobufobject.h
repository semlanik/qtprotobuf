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
#include <type_traits>

#define ASSERT_FIELD_NUMBER(X) Q_ASSERT_X(X < 128 && X > 0, T::staticMetaObject.className(), "fieldIndex is out of range")

namespace qtprotobuf {

enum WireTypes {
    Varint = 0,
    Fixed64 = 1,
    LengthDelimited = 2,
    Fixed32 = 5
};

class ProtobufObjectPrivate : public QObject {
protected:
    explicit ProtobufObjectPrivate(QObject *parent = nullptr) : QObject(parent) {}
    virtual QByteArray serializePrivate() = 0;
};

template <typename T>
class ProtobufObject : public ProtobufObjectPrivate
{
public:
    explicit ProtobufObject(QObject *parent = nullptr) : ProtobufObjectPrivate(parent) {}

    QByteArray serializePrivate() override {
        serialize();
    }

    QByteArray serialize() {
        QByteArray result;
        T* instance = dynamic_cast<T*>(this);
        for (auto field : T::propertyOrdering) {
            int propertyIndex = field.second;
            int fieldIndex = field.first;
            const char* propertyName = T::staticMetaObject.property(propertyIndex).name();
            switch (T::staticMetaObject.property(propertyIndex).type()) {
            case QMetaType::Int:
                result.append(serializeVarint(instance->property(propertyName).toInt(), fieldIndex));
                break;
            case QMetaType::Float:
                result.append(serializeFixed(instance->property(propertyName).toFloat(), fieldIndex));
                break;
            case QMetaType::Double:
                result.append(serializeFixed(instance->property(propertyName).toDouble(), fieldIndex));
                break;
            case QMetaType::QString:
                result.append(serializeLengthDelimited(instance->property(propertyName).toString().toUtf8(), fieldIndex));
                break;
            case QMetaType::QByteArray:
                result.append(serializeLengthDelimited(instance->property(propertyName).toByteArray(), fieldIndex));
                break;
            case QMetaType::User: {
//TODO: Implement recurse serialization
//                ProtobufObjectPrivate *value = instance->property(propertyName).value();
//                result.append(serializeLengthDelimited(
//                                  ->serializePrivate(),
//                                  fieldIndex));
            }
                break;
            }
        }

        return result;
    }

    void deserialize(const QByteArray& array) {
        T* instance = dynamic_cast<T*>(this);
        //TODO
    }

    QByteArray serializeLengthDelimited(const QByteArray& data, int fieldIndex) {
        ASSERT_FIELD_NUMBER(fieldIndex);
        //Varint serialize field size and apply result as starting point
        QByteArray result = serializeVarint(static_cast<unsigned int>(data.size()), 1/*field number doesn't matter*/);
        result[0] = getTypeByte(fieldIndex, LengthDelimited);
        result.append(data);
        return result;
    }

    template <typename V,
              typename std::enable_if_t<std::is_floating_point<V>::value, int> = 0>
    QByteArray serializeFixed(V value, int fieldIndex) {
        ASSERT_FIELD_NUMBER(fieldIndex);

        //Reserve required amount of bytes
        QByteArray result(sizeof(V) + 1, '\0');

        //Undentify exact wiretype
        constexpr WireTypes wireType = sizeof(V) == 4 ? Fixed32 : Fixed64;
        result[0] = getTypeByte(fieldIndex, wireType);
        *(V*)&(result.data()[1]) = value;
        return result;
    }

    template <typename V,
              typename std::enable_if_t<std::is_signed<V>::value, int> = 0>
    QByteArray serializeVarint(V value, int fieldIndex) {
        ASSERT_FIELD_NUMBER(fieldIndex);

        using UV = typename std::make_unsigned<V>::type;
        //Use ZigZag convertion first and apply unsigned variant next
        value = (value << 1) ^ (value >> (sizeof(UV) * 8 - 1));
        UV uValue = *(UV *)&value;
        return serializeVarint(uValue, fieldIndex);
    }

    template <typename V,
              typename std::enable_if_t<std::is_unsigned<V>::value, int> = 0>
    QByteArray serializeVarint(V value, int fieldIndex) {
        ASSERT_FIELD_NUMBER(fieldIndex);
        QByteArray result;
        //Reserve maximum required amount of bytes
        result.reserve(sizeof(V) + 1);
        //Put type byte at beginning
        result.append(getTypeByte(fieldIndex, Varint));

        while (value > 0) {
            //Put first 7 bits to result buffer and mark as not last
            result.append(value & 0x7F | 0x80);
            //Devide values to chunks of 7 bits, move to next chunk
            value >>= 7;
        }

        //Zero case
        if (result.size() == 1) {
            result.append('\0');
        }

        //Mark last chunk as last
        result.data()[result.size() - 1] &= ~0x80;
        return result;
    }

    inline unsigned char getTypeByte(int fieldIndex, WireTypes wireType) {
        /*  Header byte
         *  bits    | 7  6  5  4  3 |  2  1  0
         *  -----------------------------------
         *  meaning |  Field index  |   Type
         */
        unsigned char header = (fieldIndex << 3) | wireType;
        return *(char *)&header;
    }
};

}
