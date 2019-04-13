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
#include <QSharedPointer>
#include <QMetaProperty>

#include <unordered_map>
#include <memory>
#include <type_traits>
#include <functional>

#include "qtprotobuftypes.h"
#include "qtprotobuflogging.h"

#define ASSERT_FIELD_NUMBER(X) Q_ASSERT_X(X < 128 && X > 0, T::staticMetaObject.className(), "fieldIndex is out of range")

namespace qtprotobuf {

template<typename V>
struct make_unsigned { typedef typename std::make_unsigned<V>::type type; };

template<>
struct make_unsigned<int32> { typedef typename std::make_unsigned<decltype(int32::_t)>::type type; };

template<>
struct make_unsigned<int64> { typedef typename std::make_unsigned<decltype(int64::_t)>::type type; };

class ProtobufObjectPrivate
{
public:
    using Serializer = std::function<QByteArray(const QVariant &, int &)>;
    using Deserializer = std::function<void(QByteArray::const_iterator &, QVariant &)>;
    struct SerializationHandlers {
        Serializer serializer;
        Deserializer deserializer;
        WireTypes type;
    };
    using SerializerRegistry = std::unordered_map<int/*metatypeid*/, SerializationHandlers>;
    static SerializerRegistry serializers;

    ProtobufObjectPrivate() {}

    static void registerSerializers();

    template <typename T,
              typename std::enable_if_t<!std::is_base_of<QObject, T>::value, int> = 0>
    static void wrapSerializer(std::function<QByteArray(const T &, int &)> s, std::function<QVariant(QByteArray::const_iterator &)> d, WireTypes type)
    {
        serializers[qMetaTypeId<T>()] = {
            [s](const QVariant &value, int &fieldIndex) {
                return s(value.value<T>(), fieldIndex);
            },
            [d](QByteArray::const_iterator &it, QVariant & value){
                value = d(it);
            },
            type
        };
    }

    template <typename T,
              typename std::enable_if_t<!std::is_base_of<QObject, T>::value, int> = 0>
    static void wrapSerializer(std::function<QByteArray(const T &, int &)> s, std::function<void(QByteArray::const_iterator &it, QVariant & value)> d, WireTypes type)
    {
        serializers[qMetaTypeId<T>()] = {
            [s](const QVariant &value, int &fieldIndex) {
                return s(value.value<T>(), fieldIndex);
            },
            d,
            type
        };
    }

    template <typename T,
              typename std::enable_if_t<std::is_base_of<QObject, T>::value, int> = 0>
    static void wrapSerializer(std::function<QByteArray(const T &, int &)> s, std::function<QVariant(QByteArray::const_iterator &)> d, WireTypes type)
    {
        serializers[qMetaTypeId<T*>()] = {
            [s](const QVariant &value, int &fieldIndex) {
                return s(*(value.value<T*>()), fieldIndex);
            },
            [d](QByteArray::const_iterator &it, QVariant &value){
                value = d(it);
            },
            type
        };
    }

    template <typename T,
              typename std::enable_if_t<std::is_base_of<QObject, T>::value, int> = 0>
    static void wrapSerializer(std::function<QByteArray(const T &, int &)> s, std::function<void(QByteArray::const_iterator &it, QVariant &value)> d, WireTypes type)
    {
        serializers[qMetaTypeId<T*>()] = {
            [s](const QVariant &value, int &fieldIndex) {
                return s(*(value.value<T*>()), fieldIndex);
            },
            d,
            type
        };
    }

    static unsigned char encodeHeaderByte(int fieldIndex, WireTypes wireType);
    static bool decodeHeaderByte(unsigned char typeByte, int &fieldIndex, WireTypes &wireType);

    static QByteArray serializeValue(const QVariant &propertyValue, int fieldIndex, const QMetaProperty &metaProperty);
    static QByteArray serializeUserType(const QVariant &propertyValue, int &fieldIndex, WireTypes &type);

    static void deserializeProperty(QObject *object, WireTypes wireType, const QMetaProperty &metaProperty, QByteArray::const_iterator &it);
    static void deserializeUserType(const QMetaProperty &metaType, QByteArray::const_iterator &it, QVariant &newValue);

    //###########################################################################
    //                           Serialization helpers
    //###########################################################################
    static void prependLengthDelimitedSize(QByteArray &serializedList)
    {
        int empty = NotUsedFieldIndex;
        auto result = serializeBasic(static_cast<uint32_t>(serializedList.size()), empty);
        //Zero case.
        if (result.isEmpty()) {
            result.append('\0');
        }
        serializedList.prepend(result);
    }

    //----------------Serialize basic integral and floating point----------------
    template <typename V,
              typename std::enable_if_t<std::is_floating_point<V>::value
                                        || std::is_same<V, fixed32>::value
                                        || std::is_same<V, fixed64>::value
                                        || std::is_same<V, sfixed32>::value
                                        || std::is_same<V, sfixed64>::value, int> = 0>
    static QByteArray serializeBasic(V value, int &/*outFieldIndex*/) {
        qProtoDebug() << __func__ << "value" << value;

        //Reserve required amount of bytes
        QByteArray result(sizeof(V), '\0');
        *(V*)(result.data()) = value;
        return result;
    }

    template <typename V,
              typename std::enable_if_t<std::is_integral<V>::value
                                        && std::is_signed<V>::value, int> = 0>
    static QByteArray serializeBasic(V value, int &outFieldIndex) {
        qProtoDebug() << __func__ << "value" << value;
        using UV = typename qtprotobuf::make_unsigned<V>::type;
        UV uValue = 0;

        //Use ZigZag convertion first and apply unsigned variant next
        value = (value << 1) ^ (value >> (sizeof(UV) * 8 - 1));
        uValue = static_cast<UV>(value);
        return serializeBasic(uValue, outFieldIndex);
    }

    template <typename V,
              typename std::enable_if_t<std::is_same<V, int32>::value
                                        || std::is_same<V, int64>::value, int> = 0>
    static QByteArray serializeBasic(V value, int &outFieldIndex) {
        qProtoDebug() << __func__ << "value" << value;
        using UV = typename qtprotobuf::make_unsigned<V>::type;
        return serializeBasic(static_cast<UV>(value), outFieldIndex);
    }

    template <typename V,
              typename std::enable_if_t<std::is_integral<V>::value
                                        && std::is_unsigned<V>::value, int> = 0>
    static QByteArray serializeBasic(V value, int &outFieldIndex) {
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

        /* NOTE: Zero case aligned to reference cpp implementation. Where 0 ignored.
         * if (result.isEmpty()) {
         *     result.append('\0');
         * }
         */

        //Mark last chunk as last or invalidate field in case if serialized result is empty
        if (result.size() == 0) {
            outFieldIndex = NotUsedFieldIndex;
        } else {
            result.data()[result.size() - 1] &= ~0x80;
        }
        return result;
    }

    //----------------Serialize length delimited bytes and strings---------------
    static QByteArray serializeLengthDelimited(const QString &data) {
        qProtoDebug() << __func__ << "data.size" << data.size() << "data" << data;
        return serializeLengthDelimited(data.toUtf8());
    }

    static QByteArray serializeLengthDelimited(const QByteArray &data) {
        qProtoDebug() << __func__ << "data.size" << data.size() << "data" << data.toHex();
        QByteArray result(data);
        //Varint serialize field size and apply result as starting point
        prependLengthDelimitedSize(result);
        return result;
    }

    //------------------------Serialize lists of any type------------------------
    template<typename V,
             typename std::enable_if_t<!(std::is_same<V, QString>::value
                                       || std::is_same<V, QByteArray>::value
                                       || std::is_base_of<QObject, V>::value), int> = 0>
    static QByteArray serializeListType(const QList<V> &listValue, int &outFieldIndex) {
        qProtoDebug() << __func__ << "listValue.count" << listValue.count() << "outFiledIndex" << outFieldIndex;

        if (listValue.count() <= 0) {
            outFieldIndex = NotUsedFieldIndex;
            return QByteArray();
        }

        int empty = NotUsedFieldIndex;
        QByteArray serializedList;
        for (auto &value : listValue) {
            serializedList.append(serializeBasic<V>(value, empty));
        }
        //If internal field type is not LengthDelimited, exact amount of fields to be specified
        prependLengthDelimitedSize(serializedList);
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
             typename std::enable_if_t<std::is_base_of<QObject, V>::value, int> = 0>
    static QByteArray serializeListType(const QList<QSharedPointer<V>> &listValue, int &outFieldIndex) {
        qProtoDebug() << __func__ << "listValue.count" << listValue.count() << "outFiledIndex" << outFieldIndex;

        if (listValue.count() <= 0) {
            outFieldIndex = NotUsedFieldIndex;
            return QByteArray();
        }

        QByteArray serializedList;
        for (auto &value : listValue) {
            QByteArray serializedValue = serializeLengthDelimited(value->serialize());
            serializedValue.prepend(encodeHeaderByte(outFieldIndex, LengthDelimited));
            serializedList.append(serializedValue);
        }

        outFieldIndex = NotUsedFieldIndex;

        return serializedList;
    }

    //-------------------------Serialize maps of any type------------------------
    template<typename K, typename V,
             typename std::enable_if_t<!std::is_base_of<QObject, V>::value, int> = 0>
    static QByteArray serializeMap(const QMap<K,V> &mapValue, int &outFieldIndex) {
        using ItType = typename QMap<K,V>::const_iterator;
        QByteArray mapResult;
        auto kSerializer = serializers[qMetaTypeId<K>()];
        auto vSerializer = serializers[qMetaTypeId<V>()];

        for ( ItType it = mapValue.constBegin(); it != mapValue.constEnd(); it++) {
            QByteArray result;
            result = mapSerializeHelper<K, 1>(it.key(), kSerializer) + mapSerializeHelper<V, 2>(it.value(), vSerializer);
            prependLengthDelimitedSize(result);
            result.prepend(encodeHeaderByte(outFieldIndex, LengthDelimited));
            mapResult.append(result);
        }
        outFieldIndex = NotUsedFieldIndex;
        return mapResult;
    }

    template<typename K, typename V,
             typename std::enable_if_t<std::is_base_of<QObject, V>::value, int> = 0>
    static QByteArray serializeMap(const QMap<K, QSharedPointer<V>> &mapValue, int &outFieldIndex) {
        using ItType = typename QMap<K, QSharedPointer<V>>::const_iterator;
        QByteArray mapResult;
        auto kSerializer = serializers[qMetaTypeId<K>()];
        auto vSerializer = serializers[qMetaTypeId<V*>()];

        for ( ItType it = mapValue.constBegin(); it != mapValue.constEnd(); it++) {
            QByteArray result;
            if (it.value().isNull()) {
                qProtoWarning() << __func__ << "Trying to serialize map value that contains nullptr";
                continue;
            }
            result = mapSerializeHelper<K, 1>(it.key(), kSerializer) + mapSerializeHelper<V, 2>(it.value().data(), vSerializer);
            prependLengthDelimitedSize(result);
            result.prepend(encodeHeaderByte(outFieldIndex, LengthDelimited));
            mapResult.append(result);
        }
        outFieldIndex = NotUsedFieldIndex;
        return mapResult;
    }

    template <typename V, int num,
              typename std::enable_if_t<!std::is_base_of<QObject, V>::value, int> = 0>
    static QByteArray mapSerializeHelper(const V &value, const SerializationHandlers &handlers) {
        int mapIndex = num;
        QByteArray result = handlers.serializer(QVariant::fromValue<V>(value), mapIndex);
        if (mapIndex != NotUsedFieldIndex
                && handlers.type != UnknownWireType) {
            result.prepend(encodeHeaderByte(mapIndex, handlers.type));
        }
        return result;
    }

    template <typename V, int num,
              typename std::enable_if_t<std::is_base_of<QObject, V>::value, int> = 0>
    static QByteArray mapSerializeHelper(V *value, const SerializationHandlers &handlers) {
        int mapIndex = num;
        QByteArray result = handlers.serializer(QVariant::fromValue<V*>(value), mapIndex);
        if (mapIndex != NotUsedFieldIndex
                && handlers.type != UnknownWireType) {
            result.prepend(encodeHeaderByte(mapIndex, handlers.type));
        }
        return result;
    }

    //###########################################################################
    //                           Deserialization helpers
    //###########################################################################
    template <typename V,
              typename std::enable_if_t<std::is_floating_point<V>::value
                                        || std::is_same<V, sfixed32>::value
                                        || std::is_same<V, sfixed64>::value
                                        || std::is_same<V, fixed32>::value
                                        || std::is_same<V, fixed64>::value, int> = 0>
    static uint32 getRepeatedFieldCount(QByteArray::const_iterator &it) {
        return deserializeBasic<uint32>(it).value<uint32>() / sizeof(V);
    }

    template <typename V,
              typename std::enable_if_t<std::is_integral<V>::value
                                        || std::is_same<V, sint32>::value
                                        || std::is_same<V, sint64>::value, int> = 0>
    static uint32 getRepeatedFieldCount(QByteArray::const_iterator &it) {
        return deserializeBasic<uint32>(it).value<uint32>();
    }

    //---------------Deserialize basic integral and floating point---------------
    template <typename V,
              typename std::enable_if_t<std::is_floating_point<V>::value
                                        || std::is_same<V, fixed32>::value
                                        || std::is_same<V, fixed64>::value
                                        || std::is_same<V, sfixed32>::value
                                        || std::is_same<V, sfixed64>::value, int> = 0>
    static QVariant deserializeBasic(QByteArray::const_iterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        QVariant newPropertyValue(QVariant::fromValue(*(V*)it));
        it += sizeof(V);
        return newPropertyValue;
    }

    template <typename V,
              typename std::enable_if_t<std::is_integral<V>::value
                                        && std::is_unsigned<V>::value, int> = 0>
    static QVariant deserializeBasic(QByteArray::const_iterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        return QVariant::fromValue(deserializeVarintCommon<V>(it));
    }

    template <typename V,
              typename std::enable_if_t<std::is_integral<V>::value
                                        && std::is_signed<V>::value,int> = 0>
    static QVariant deserializeBasic(QByteArray::const_iterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);
        using  UV = typename qtprotobuf::make_unsigned<V>::type;
        UV unsignedValue = deserializeVarintCommon<UV>(it);
        V value = (unsignedValue >> 1) ^ (-(unsignedValue & 1));
        return QVariant::fromValue<V>(value);
    }

    template <typename V,
              typename std::enable_if_t<std::is_same<int32, V>::value
                                        || std::is_same<int64, V>::value, int> = 0>
    static QVariant deserializeBasic(QByteArray::const_iterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);
        using  UV = typename qtprotobuf::make_unsigned<V>::type;
        UV unsignedValue = deserializeVarintCommon<UV>(it);
        V value = static_cast<V>(unsignedValue);
        return QVariant::fromValue(value);
    }

    template <typename V,
              typename std::enable_if_t<std::is_integral<V>::value
                                        && std::is_unsigned<V>::value, int> = 0>
    static V deserializeVarintCommon(QByteArray::const_iterator &it) {
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

    //---------------Deserialize length delimited bytes and strings--------------
    static QByteArray deserializeLengthDelimited(QByteArray::const_iterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        unsigned int length = deserializeBasic<uint32>(it).toUInt();
        QByteArray result(it, length);
        it += length;
        return result;
    }

    //-----------------------Deserialize lists of any type-----------------------
    template <typename V,
              typename std::enable_if_t<std::is_same<V, QString>::value
                                        || std::is_same<V, QByteArray>::value, int> = 0>
    static QVariant deserializeList(QByteArray::const_iterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        return QVariant::fromValue(deserializeLengthDelimited(it));
    }

    template <typename V,
              typename std::enable_if_t<std::is_base_of<QObject, V>::value, int> = 0>
    static QVariant deserializeList(QByteArray::const_iterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);
        QVariant newValue;
        serializers[qMetaTypeId<V*>()].deserializer(it, newValue);
        return newValue;
    }

    template <typename V,
              typename std::enable_if_t<!(std::is_same<V, QString>::value
                                        || std::is_same<V, QByteArray>::value
                                        || std::is_base_of<QObject, V>::value), int> = 0>
    static QVariant deserializeList(QByteArray::const_iterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        QList<V> out;
        unsigned int count = deserializeBasic<uint32>(it).toUInt();
        QByteArray::const_iterator lastVarint = it + count;
        while (it != lastVarint) {
            QVariant variant = deserializeBasic<V>(it);
            out.append(variant.value<V>());
        }
        return QVariant::fromValue(out);
    }

    //-----------------------Deserialize maps of any type------------------------
    template <typename K, typename V,
              typename std::enable_if_t<!std::is_base_of<QObject, V>::value, int> = 0>
    static void deserializeMap(QByteArray::const_iterator &it, QVariant &previous) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);
        QMap<K,V> out = previous.value<QMap<K,V>>();

        int mapIndex = 0;
        WireTypes type = WireTypes::UnknownWireType;

        K key;
        V value;

        unsigned int count = deserializeBasic<uint32>(it).toUInt();
        qProtoDebug() << __func__ << "count:" << count;
        QByteArray::const_iterator last = it + count;
        while (it != last) {
            decodeHeaderByte(*it, mapIndex, type);
            ++it;
            if(mapIndex == 1) {
                key = deserializeMapHelper<K>(it);
            } else {
                value = deserializeMapHelper<V>(it);
            }
        }

        out[key] = value;
        previous = QVariant::fromValue<QMap<K,V>>(out);
    }

    template <typename K, typename V,
              typename std::enable_if_t<std::is_base_of<QObject, V>::value, int> = 0>
    static void deserializeMap(QByteArray::const_iterator &it, QVariant &previous) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);
        auto out = previous.value<QMap<K, QSharedPointer<V>>>();

        int mapIndex = 0;
        WireTypes type = WireTypes::UnknownWireType;

        K key;
        V* value;

        unsigned int count = deserializeBasic<uint32>(it).toUInt();
        qProtoDebug() << __func__ << "count:" << count;
        QByteArray::const_iterator last = it + count;
        while (it != last) {
            decodeHeaderByte(*it, mapIndex, type);
            ++it;
            if(mapIndex == 1) {
                key = deserializeMapHelper<K>(it);
            } else {
                value = deserializeMapHelper<V>(it);
            }
        }

        out[key] = QSharedPointer<V>(value);
        previous = QVariant::fromValue<QMap<K,QSharedPointer<V>>>(out);
    }

    template <typename T,
              typename std::enable_if_t<std::is_base_of<QObject, T>::value, int> = 0>
    static T *deserializeMapHelper(QByteArray::const_iterator &it) {
        auto serializer = serializers[qMetaTypeId<T *>()];
        QVariant value;
        serializer.deserializer(it, value);
        return value.value<T *>();
    }

    template <typename T,
              typename std::enable_if_t<!std::is_base_of<QObject, T>::value, int> = 0>
    static T deserializeMapHelper(QByteArray::const_iterator &it) {
        auto serializer = serializers[qMetaTypeId<T>()];
        QVariant value;
        serializer.deserializer(it, value);
        return value.value<T>();
    }

    //-----------------------Functions to work with objects------------------------
    template<typename T>
    static void registerSerializers() {
        ProtobufObjectPrivate::wrapSerializer<T>(serializeComplexType<T>, deserializeComplexType<T>, LengthDelimited);
        ProtobufObjectPrivate::serializers[qMetaTypeId<QList<QSharedPointer<T>>>()] = {ProtobufObjectPrivate::Serializer(serializeComplexListType<T>),
                ProtobufObjectPrivate::Deserializer(deserializeComplexListType<T>), LengthDelimited};
    }

    template <typename T,
              typename std::enable_if_t<std::is_base_of<QObject, T>::value, int> = 0>
    static QByteArray serializeComplexType(const T &value, int &/*outFieldIndex*/) {
        return ProtobufObjectPrivate::serializeLengthDelimited(value.serialize());
    }

    template <typename T,
              typename std::enable_if_t<std::is_base_of<QObject, T>::value, int> = 0>
    static QVariant deserializeComplexType(QByteArray::const_iterator &it) {
        T *value = new T;
        value->deserialize(ProtobufObjectPrivate::deserializeLengthDelimited(it));
        return QVariant::fromValue<T*>(value);
    }

    template <typename T,
              typename std::enable_if_t<std::is_base_of<QObject, T>::value, int> = 0>
    static QByteArray serializeComplexListType(const QVariant &listValue, int &outFieldIndex) {
        QList<QSharedPointer<T>> list = listValue.value<QList<QSharedPointer<T>>>();
        return ProtobufObjectPrivate::serializeListType(list, outFieldIndex);
    }

    template <typename T,
              typename std::enable_if_t<std::is_base_of<QObject, T>::value, int> = 0>
    static void deserializeComplexListType(QByteArray::const_iterator &it, QVariant &previous) {
        QList<QSharedPointer<T>> previousList = previous.value<QList<QSharedPointer<T>>>();
        QVariant newMember = ProtobufObjectPrivate::deserializeList<T>(it);
        previousList.append(QSharedPointer<T>(newMember.value<T*>()));
        previous.setValue(previousList);
    }

    template<typename T>
    static QByteArray serialize(const QObject *object) {
        qProtoDebug() << T::staticMetaObject.className() << "serialize";

        QByteArray result;
        for (auto field : T::propertyOrdering) {
            int propertyIndex = field.second;
            int fieldIndex = field.first;
            ASSERT_FIELD_NUMBER(fieldIndex);
            QMetaProperty metaProperty = T::staticMetaObject.property(propertyIndex);
            const char *propertyName = metaProperty.name();
            const QVariant &propertyValue = object->property(propertyName);
            result.append(ProtobufObjectPrivate::serializeValue(propertyValue, fieldIndex, metaProperty));
        }

        return result;
    }

    template<typename T>
    static void deserialize(QObject* object, const QByteArray &array) {
        qProtoDebug() << T::staticMetaObject.className() << "deserialize";

        for (QByteArray::const_iterator it = array.begin(); it != array.end();) {
            //Each iteration we expect iterator is setup to beginning of next chunk
            int fieldNumber = NotUsedFieldIndex;
            WireTypes wireType = UnknownWireType;
            if (!ProtobufObjectPrivate::decodeHeaderByte(*it, fieldNumber, wireType)) {
                qProtoCritical() << "Message received doesn't contains valid header byte. "
                               "Trying next, but seems stream is broken" << QString::number((*it), 16);
                ++it;
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
            ProtobufObjectPrivate::deserializeProperty(object, wireType, metaProperty, it);
        }
    }
};


/*  Header byte
 *  bits    | 7  6  5  4  3 | 2  1  0
 *  -----------------------------------
 *  meaning |  Field index  |  Type
 */
inline unsigned char ProtobufObjectPrivate::encodeHeaderByte(int fieldIndex, WireTypes wireType)
{
    unsigned char header = (fieldIndex << 3) | wireType;
    return static_cast<char>(header);
}

inline bool ProtobufObjectPrivate::decodeHeaderByte(unsigned char typeByte, int &fieldIndex, WireTypes &wireType)
{
    wireType = static_cast<WireTypes>(typeByte & 0x07);
    fieldIndex = typeByte >> 3;
    return fieldIndex < 128 && fieldIndex > 0 && (wireType == Varint
                                                  || wireType == Fixed64
                                                  || wireType == Fixed32
                                                  || wireType == LengthDelimited);
}

}
