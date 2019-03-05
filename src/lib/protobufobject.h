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

namespace qtprotobuf {

enum WireTypes {
    Varint = 0,
    Fixed64 = 1,
    LengthDelimited = 2,
    Fixed32 = 5
};

template <typename T>
class ProtobufObject : public QObject
{
public:
    explicit ProtobufObject(QObject *parent = nullptr) : QObject(parent)
    {}

    QByteArray serialize() {
        QByteArray result;
        T* instance = dynamic_cast<T*>(this);
        for(auto field : T::propertyOrdering) {
            int propertyIndex = field.second;
            int fieldIndex = field.first;
            const char* propertyName = T::staticMetaObject.property(propertyIndex).name();
            switch(T::staticMetaObject.property(propertyIndex).type()) {
            case QVariant::Int:
                result.append(serializeVarint(instance->property(propertyName).toInt(), fieldIndex));
                break;
            }
        }

        return result;
    }

    void deserialize(const QByteArray& array) {
        T* instance = dynamic_cast<T*>(this);
        //TODO
    }

    template <typename V,
              typename std::enable_if_t<std::is_signed<V>::value, int> = 0>
    QByteArray serializeVarint(V value, int fieldIndex) {
        using UV = typename std::make_unsigned<V>::type;
        //Use ZigZag convertion first and apply unsigned variant next
        value = (value << 1) ^ (value >> (sizeof(UV) * 8 - 1));
        UV uValue = *(UV *)&value;
        return serializeVarint(uValue, fieldIndex);
    }

    template <typename V,
              typename std::enable_if_t<std::is_unsigned<V>::value, int> = 0>
    QByteArray serializeVarint(V value, int fieldIndex) {
        /*  Header byte
         *  bits    | 7  6  5  4  3 |  2  1  0
         *  -----------------------------------
         *  meaning |  Field index  |   Type
         */
        Q_ASSERT_X(fieldIndex < 128 && fieldIndex > 0, T::staticMetaObject.className(), "fieldIndex is out of range");

        unsigned char typeByte = (fieldIndex << 3) | Varint;
        QByteArray result;
        //Reserve maximum required amount of bytes
        result.reserve(sizeof(V) + 1);
        //Put type byte at beginning
        result.append(*(char *)&typeByte);

        while(value > 0) {
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
};

}
