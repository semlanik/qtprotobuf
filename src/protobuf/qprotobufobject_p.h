/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>, Viktor Kopp <vifactor@gmail.com>
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
#include <QQmlListProperty>
#include <QQmlEngine>

#include <unordered_map>
#include <memory>
#include <type_traits>
#include <functional>

#include "qtprotobuftypes.h"
#include "qtprotobuflogging.h"
#include "selfcheckiterator.h"

#include "qtprotobuf_global.h"

#define ASSERT_FIELD_NUMBER(X) Q_ASSERT_X(X < 536870912 && X > 0, T::staticMetaObject.className(), "fieldIndex is out of range")

namespace qtprotobuf {

template <typename V,
          typename std::enable_if_t<std::is_integral<V>::value
                                    && std::is_unsigned<V>::value, int> = 0>
static V deserializeVarintCommon(SelfcheckIterator &it) {
    qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

    V value = 0;
    int k = 0;
    while (true) {
        uint64_t byte = static_cast<uint64_t>(*it);
        value += (byte & 0b01111111) << k;
        k += 7;
        if (((*it) & 0b10000000) == 0) {
            break;
        }
        ++it;
    }
    ++it;
    return value;
}

template <typename V,
          typename std::enable_if_t<std::is_integral<V>::value
                                    && std::is_unsigned<V>::value, int> = 0>
static QByteArray serializeVarintCommon(const V &value) {
    qProtoDebug() << __func__ << "value" << value;
    V varint = value;
    QByteArray result;

    while (varint != 0) {
        //Put 7 bits to result buffer and mark as "not last" (0b10000000)
        result.append((varint & 0b01111111) | 0b10000000);
        //Divide values to chunks of 7 bits and move to next chunk
        varint >>= 7;
    }

    if (result.isEmpty()) {
        result.append('\0');
    }

    result.data()[result.size() - 1] &= ~0b10000000;
    return result;
}

class QTPROTOBUFSHARED_EXPORT ProtobufObjectPrivate
{
    ProtobufObjectPrivate() = delete;
    ~ProtobufObjectPrivate() = delete;
    Q_DISABLE_COPY(ProtobufObjectPrivate)
    ProtobufObjectPrivate(ProtobufObjectPrivate &&) = delete;
    ProtobufObjectPrivate &operator =(ProtobufObjectPrivate &&) = delete;
public:
    using Serializer = std::function<QByteArray(const QVariant &, int &)>;
    using Deserializer = std::function<void(SelfcheckIterator &, QVariant &)>;
    struct SerializationHandlers {
        Serializer serializer;
        Deserializer deserializer;
        WireTypes type;
    };
    using SerializerRegistry = std::unordered_map<int/*metatypeid*/, SerializationHandlers>;
    static SerializerRegistry serializers;

    static void registerSerializers();

    static QByteArray encodeHeader(int fieldIndex, WireTypes wireType);
    static bool decodeHeader(SelfcheckIterator &it, int &fieldIndex, WireTypes &wireType);

    static QByteArray serializeProperty(const QVariant &propertyValue, int fieldIndex, const QMetaProperty &metaProperty);
    static QByteArray serializeUserType(const QVariant &propertyValue, int &fieldIndex, WireTypes &type);

    static void deserializeProperty(QObject *object, const QMetaProperty &metaProperty, SelfcheckIterator &it, WireTypes wireType);
    static void deserializeUserType(const QMetaProperty &metaProperty, SelfcheckIterator &it, QVariant &out);

    //###########################################################################
    //                           Serialization helpers
    //###########################################################################

    /**
     * @brief Gets length of a byte-array and prepends to it its serialized length value
     *      using the appropriate serialization algorithm
     *
     *
     * @param[in, out] serializedList Byte-array to be prepended
     */
    static void prependLengthDelimitedSize(QByteArray &serializedList)
    {
        serializedList.prepend(serializeVarintCommon<uint32_t>(serializedList.size()));
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
             typename std::enable_if_t<std::is_base_of<QObject, V>::value, int> = 0>
    static QByteArray serializeListType(const QList<QSharedPointer<V>> &listValue, int &outFieldIndex) {
        qProtoDebug() << __func__ << "listValue.count" << listValue.count() << "outFiledIndex" << outFieldIndex;

        if (listValue.count() <= 0) {
            outFieldIndex = NotUsedFieldIndex;
            return QByteArray();
        }

        QByteArray serializedList;
        for (auto &value : listValue) {
            if (!value) {
                qProtoWarning() << "Null pointer in list";
                continue;
            }
            serializedList.append(encodeHeader(outFieldIndex, LengthDelimited));
            serializedList.append(serializeLengthDelimited(serializeObject<V>(value.data())));
        }

        outFieldIndex = NotUsedFieldIndex;

        return serializedList;
    }

    //-------------------------Serialize maps of any type------------------------
    template<typename K, typename V,
             typename std::enable_if_t<!std::is_base_of<QObject, V>::value, int> = 0>
    static QByteArray serializeMap(const QVariant &value, int &outFieldIndex) {
        QMap<K,V> mapValue = value.value<QMap<K,V>>();
        using ItType = typename QMap<K,V>::const_iterator;
        QByteArray mapResult;
        auto kSerializer = serializers.at(qMetaTypeId<K>());//Throws exception if not found
        auto vSerializer = serializers.at(qMetaTypeId<V>());//Throws exception if not found

        for ( ItType it = mapValue.constBegin(); it != mapValue.constEnd(); it++) {
            QByteArray result;
            result = mapSerializeHelper<K, 1>(it.key(), kSerializer) + mapSerializeHelper<V, 2>(it.value(), vSerializer);
            prependLengthDelimitedSize(result);
            result.prepend(encodeHeader(outFieldIndex, LengthDelimited));
            mapResult.append(result);
        }
        outFieldIndex = NotUsedFieldIndex;
        return mapResult;
    }

    template<typename K, typename V,
             typename std::enable_if_t<std::is_base_of<QObject, V>::value, int> = 0>
    static QByteArray serializeMap(const QVariant &value, int &outFieldIndex) {
        QMap<K, QSharedPointer<V>> mapValue = value.value<QMap<K, QSharedPointer<V>>>();
        using ItType = typename QMap<K, QSharedPointer<V>>::const_iterator;
        QByteArray mapResult;
        auto kSerializer = serializers.at(qMetaTypeId<K>());//Throws exception if not found
        auto vSerializer = serializers.at(qMetaTypeId<V *>());//Throws exception if not found

        for ( ItType it = mapValue.constBegin(); it != mapValue.constEnd(); it++) {
            QByteArray result;
            if (it.value().isNull()) {
                qProtoWarning() << __func__ << "Trying to serialize map value that contains nullptr";
                continue;
            }
            result = mapSerializeHelper<K, 1>(it.key(), kSerializer) + mapSerializeHelper<V, 2>(it.value().data(), vSerializer);
            prependLengthDelimitedSize(result);
            result.prepend(encodeHeader(outFieldIndex, LengthDelimited));
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
            result.prepend(encodeHeader(mapIndex, handlers.type));
        }
        return result;
    }

    template <typename V, int num,
              typename std::enable_if_t<std::is_base_of<QObject, V>::value, int> = 0>
    static QByteArray mapSerializeHelper(V *value, const SerializationHandlers &handlers) {
        int mapIndex = num;
        QByteArray result = handlers.serializer(QVariant::fromValue<V *>(value), mapIndex);
        if (mapIndex != NotUsedFieldIndex
                && handlers.type != UnknownWireType) {
            result.prepend(encodeHeader(mapIndex, handlers.type));
        }
        return result;
    }

    //###########################################################################
    //                           Deserialization helpers
    //###########################################################################
    //---------------Deserialize length delimited bytes and strings--------------
    static QByteArray deserializeLengthDelimited(SelfcheckIterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        unsigned int length = deserializeVarintCommon<uint32>(it);
        QByteArray result((QByteArray::const_iterator&)it, length);
        it += length;
        return result;
    }

    //-----------------------Deserialize lists of any type-----------------------
    template <typename V,
              typename std::enable_if_t<std::is_base_of<QObject, V>::value, int> = 0>
    static void deserializeList(SelfcheckIterator &it, QVariant &previous) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        QVariant newValue;
        QList<QSharedPointer<V>> list = previous.value<QList<QSharedPointer<V>>>();
        serializers.at(qMetaTypeId<V *>()).deserializer(it, newValue);//Throws exception if not found
        list.append(QSharedPointer<V>(newValue.value<V *>()));
        previous.setValue(list);
    }

    //-----------------------Deserialize maps of any type------------------------
    template <typename K, typename V,
              typename std::enable_if_t<!std::is_base_of<QObject, V>::value, int> = 0>
    static void deserializeMap(SelfcheckIterator &it, QVariant &previous) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);
        QMap<K,V> out = previous.value<QMap<K,V>>();

        int mapIndex = 0;
        WireTypes type = WireTypes::UnknownWireType;

        K key;
        V value;

        unsigned int count = deserializeVarintCommon<uint32>(it);
        qProtoDebug() << __func__ << "count:" << count;
        SelfcheckIterator last = it + count;
        while (it != last) {
            decodeHeader(it, mapIndex, type);
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
    static void deserializeMap(SelfcheckIterator &it, QVariant &previous) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);
        auto out = previous.value<QMap<K, QSharedPointer<V>>>();

        int mapIndex = 0;
        WireTypes type = WireTypes::UnknownWireType;

        K key;
        V *value;

        unsigned int count = deserializeVarintCommon<uint32>(it);
        qProtoDebug() << __func__ << "count:" << count;
        SelfcheckIterator last = it + count;
        while (it != last) {
            decodeHeader(it, mapIndex, type);
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
    static T *deserializeMapHelper(SelfcheckIterator &it) {
        auto serializer = serializers.at(qMetaTypeId<T *>());//Throws exception if not found
        QVariant value;
        serializer.deserializer(it, value);
        return value.value<T *>();
    }

    template <typename T,
              typename std::enable_if_t<!std::is_base_of<QObject, T>::value, int> = 0>
    static T deserializeMapHelper(SelfcheckIterator &it) {
        auto serializer = serializers.at(qMetaTypeId<T>());//Throws exception if not found
        QVariant value;
        serializer.deserializer(it, value);
        return value.value<T>();
    }

    //-----------------------Functions to work with objects------------------------
    template<typename T>
    static void registerSerializers() {
        ProtobufObjectPrivate::serializers[qMetaTypeId<T *>()] = {ProtobufObjectPrivate::Serializer(serializeComplexType<T>),
                ProtobufObjectPrivate::Deserializer(deserializeComplexType<T>), LengthDelimited};
        ProtobufObjectPrivate::serializers[qMetaTypeId<QList<QSharedPointer<T>>>()] = {ProtobufObjectPrivate::Serializer(serializeComplexListType<T>),
                ProtobufObjectPrivate::Deserializer(deserializeComplexListType<T>), LengthDelimited};
    }

    template<typename K, typename V,
             typename std::enable_if_t<!std::is_base_of<QObject, V>::value, int> = 0>
    static void registerMap() {
        ProtobufObjectPrivate::serializers[qMetaTypeId<QMap<K, V>>()] = {qtprotobuf::ProtobufObjectPrivate::serializeMap<K, V>,
        qtprotobuf::ProtobufObjectPrivate::deserializeMap<K, V>, LengthDelimited};
    }

    template<typename K, typename V,
             typename std::enable_if_t<std::is_base_of<QObject, V>::value, int> = 0>
    static void registerMap() {
        ProtobufObjectPrivate::serializers[qMetaTypeId<QMap<K, QSharedPointer<V>>>()] = {qtprotobuf::ProtobufObjectPrivate::serializeMap<K, V>,
        qtprotobuf::ProtobufObjectPrivate::deserializeMap<K, V>, LengthDelimited};
    }

    template <typename T,
              typename std::enable_if_t<std::is_base_of<QObject, T>::value, int> = 0>
    static QByteArray serializeComplexType(const QVariant &value, int &/*outFieldIndex*/) {
        return ProtobufObjectPrivate::serializeLengthDelimited(serializeObject<T>(value.value<T *>()));
    }

    template <typename T,
              typename std::enable_if_t<std::is_base_of<QObject, T>::value, int> = 0>
    static void deserializeComplexType(SelfcheckIterator &it, QVariant &to) {
        T *value = new T;
        deserializeObject<T>(value, ProtobufObjectPrivate::deserializeLengthDelimited(it));
        to = QVariant::fromValue<T *>(value);
    }

    template <typename T,
              typename std::enable_if_t<std::is_base_of<QObject, T>::value, int> = 0>
    static QByteArray serializeComplexListType(const QVariant &listValue, int &outFieldIndex) {
        QList<QSharedPointer<T>> list = listValue.value<QList<QSharedPointer<T>>>();
        return ProtobufObjectPrivate::serializeListType(list, outFieldIndex);
    }

    template <typename T,
              typename std::enable_if_t<std::is_base_of<QObject, T>::value, int> = 0>
    static void deserializeComplexListType(SelfcheckIterator &it, QVariant &previous) {
        ProtobufObjectPrivate::deserializeList<T>(it, previous);
    }

    /**
    * @brief Serialization of a registered qtproto message object into byte-array
    *
    *
    * @param[in] object Pointer to QObject containing message to be serialized
    * @result serialized message bytes
    */
    template<typename T>
    static QByteArray serializeObject(const QObject *object) {
        qProtoDebug() << T::staticMetaObject.className() << "serialize";

        QByteArray result;
        for (const auto field : T::propertyOrdering) {
            int propertyIndex = field.second;
            int fieldIndex = field.first;
            ASSERT_FIELD_NUMBER(fieldIndex);
            QMetaProperty metaProperty = T::staticMetaObject.property(propertyIndex);
            const char *propertyName = metaProperty.name();
            const QVariant &propertyValue = object->property(propertyName);
            result.append(ProtobufObjectPrivate::serializeProperty(propertyValue, fieldIndex, metaProperty));
        }

        return result;
    }

    // this set of 3 methods is used to skip bytes corresponding to an unexpected property
    // in a serialized message met while the message being deserialized
    static void skipVarint(SelfcheckIterator &it);
    static void skipLengthDelimited(SelfcheckIterator &it);
    static int skipSerializedFieldBytes(SelfcheckIterator &it, WireTypes type);


    /**
    * @brief Deserialization of a byte-array into a registered qtproto message object
    *
    * @details Properties in a message are identified via ProtobufObjectPrivate::decodeHeader.
    *          Bytes corresponding to unexpected properties are skipped without any exception
    *
    * @param[out] object Pointer to memory where result of deserialization should be injected
    * @param[in] array Bytes with serialized message
    */
    template<typename T>
    static void deserializeObject(QObject *object, const QByteArray &array) {
        qProtoDebug() << T::staticMetaObject.className() << "deserialize";

        for (SelfcheckIterator it(array); it != array.end();) {
            //Each iteration we expect iterator is setup to beginning of next chunk
            int fieldNumber = NotUsedFieldIndex;
            WireTypes wireType = UnknownWireType;
            if (!ProtobufObjectPrivate::decodeHeader(it, fieldNumber, wireType)) {
                qProtoCritical() << "Message received doesn't contains valid header byte. "
                                    "Trying next, but seems stream is broken" << QString::number((*it), 16);
                throw std::invalid_argument("Message received doesn't contains valid header byte. "
                                      "Seems stream is broken");
            }

            auto propertyNumberIt = T::propertyOrdering.find(fieldNumber);
            if (propertyNumberIt == std::end(T::propertyOrdering)) {
                auto bytesCount = skipSerializedFieldBytes(it, wireType);
                qProtoWarning() << "Message received contains unexpected/optional field. WireType:" << wireType
                                << ", field number: " << fieldNumber << "Skipped:" << (bytesCount + 1) << "bytes";
                continue;
            }

            int propertyIndex = propertyNumberIt->second;
            QMetaProperty metaProperty = T::staticMetaObject.property(propertyIndex);

            ProtobufObjectPrivate::deserializeProperty(object, metaProperty, it, wireType);
        }
    }

    //###########################################################################
    //                                Qml support
    //###########################################################################

    template<typename T>
    static void qmllistpropertyAppend(QQmlListProperty<T> *p, T *v) {
        QQmlEngine::setObjectOwnership(v, QQmlEngine::CppOwnership);
        reinterpret_cast<QList<QSharedPointer<T>> *>(p->data)->append(QSharedPointer<T>(v));
    }

    template<typename T>
    static int qmllistpropertyCount(QQmlListProperty<T> *p) {
        return reinterpret_cast<QList<QSharedPointer<T>> *>(p->data)->count();
    }

    template<typename T>
    static T * qmllistpropertyAt(QQmlListProperty<T> *p, int index) {
        return reinterpret_cast<QList<QSharedPointer<T>> *>(p->data)->at(index).data();
    }

    template<typename T>
    static void qmllistpropertyReset(QQmlListProperty<T> *p){
        reinterpret_cast<QList<QSharedPointer<T>> *>(p->data)->clear();
    }

    template<typename T>
    static QQmlListProperty<T> constructQmlListProperty(QObject *p, QList<QSharedPointer<T>> *data)
    {
        return QQmlListProperty<T>(p, data, qmllistpropertyAppend<T>, qmllistpropertyCount<T>,
                                   qmllistpropertyAt<T>, qmllistpropertyReset<T>);
    }
};

//###########################################################################
//                             Common functions
//###########################################################################

/*! @brief Encode a property field index and its type into output bytes
 *
 * @details
 * Header byte
 *  Meaning    |  Field index  |  Type
 *  ---------- | ------------- | --------
 *  bit number | 7  6  5  4  3 | 2  1  0
 * @param fieldIndex The index of a property in parent object
 * @param wireType Serialization type used for the property with index @p fieldIndex
 *
 * @return Varint encoded fieldIndex and wireType
 */
inline QByteArray ProtobufObjectPrivate::encodeHeader(int fieldIndex, WireTypes wireType)
{
    uint32_t header = (fieldIndex << 3) | wireType;
    return serializeVarintCommon<uint32_t>(header);
}

/*! @brief Decode a property field index and its serialization type from input bytes
 *
 * @param[in] Iterator that points to header with encoded field index and serialization type
 * @param[out] fieldIndex Decoded index of a property in parent object
 * @param[out] wireType Decoded serialization type used for the property with index @p fieldIndex
 *
 * @return true if both decoded wireType and fieldIndex have "allowed" values and false, otherwise
 */
inline bool ProtobufObjectPrivate::decodeHeader(SelfcheckIterator &it, int &fieldIndex, WireTypes &wireType)
{
    uint32_t header = deserializeVarintCommon<uint32_t>(it);
    wireType = static_cast<WireTypes>(header & 0b00000111);
    fieldIndex = header >> 3;

    constexpr int maxFieldIndex = (1 << 29) - 1;
    return fieldIndex <= maxFieldIndex && fieldIndex > 0 && (wireType == Varint
                                                  || wireType == Fixed64
                                                  || wireType == Fixed32
                                                  || wireType == LengthDelimited);
}

}
