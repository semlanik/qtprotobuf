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

#include "protobufobject_p.h"

#include <QMetaObject>
#include <QMetaProperty>

#include <unordered_map>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <functional>

#include <qtprotobuftypes.h>
#include <qtprotobuflogging.h>

#define ASSERT_FIELD_NUMBER(X) Q_ASSERT_X(X < 128 && X > 0 && X != NotUsedFieldIndex, T::staticMetaObject.className(), "fieldIndex is out of range")

namespace qtprotobuf {

template <typename T>
class ProtobufObject : public ProtobufObjectPrivate
{
public:
    explicit ProtobufObject(QObject *parent = nullptr) : ProtobufObjectPrivate(parent) {}

    QByteArray serialize() const {
        qProtoDebug() << T::staticMetaObject.className() << "serialize";

        QByteArray result;
        const QObject *instance = static_cast<const QObject *>(this);
        for (auto field : T::propertyOrdering) {
            int propertyIndex = field.second;
            int fieldIndex = field.first;
            ASSERT_FIELD_NUMBER(fieldIndex);
            QMetaProperty metaProperty = T::staticMetaObject.property(propertyIndex);
            const char *propertyName = metaProperty.name();
            const QVariant &propertyValue = instance->property(propertyName);
            result.append(serializeValue(propertyValue, fieldIndex, QLatin1Literal(metaProperty.typeName())));
        }

        return result;
    }

    void deserialize(const QByteArray &array) {
        qProtoDebug() << T::staticMetaObject.className() << "deserialize";

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
    static void registerSerializers(int /*metaTypeId*/, int listMetaTypeId) {
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

    static QByteArray serializeComplexListType(const ProtobufObjectPrivate *serializer, const QVariant &listValue, int &outFieldIndex) {
        QList<T> list = listValue.value<QList<T>>();
        return serializer->serializeListType(list, outFieldIndex);
    }

    static void deserializeComplexListType(ProtobufObjectPrivate *deserializer, QByteArray::const_iterator &it, QVariant &previous) {
        QList<T> previousList = previous.value<QList<T>>();
        QVariant newMember = deserializer->deserializeListType<T>(it);
        previousList.append(newMember.value<T>());
        previous.setValue(previousList);
    }
};

}
