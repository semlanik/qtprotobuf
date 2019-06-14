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

#include <memory>
#include <type_traits>

#include "qtprotobuflogging.h"
#include "selfcheckiterator.h"
#include "qabstractprotobufserializer.h"

#include "qtprotobuf_global.h"

namespace qtprotobuf {
/**
*  \addtogroup QtProtobuf
*  @{
*/

/**
 * @brief The QProtobufSerializerRegistry class provides api to register serializers of QObjects
 *
 * @details The QProtobufSerializerRegistry class registers serializers/deserializers for classes inherited of QObject.
 *          To register serializers for user-defined class it has to be inherited of QObjec contain macro's.
 *          @code{.cpp}
 *          class MyType : public QObject
 *          {
 *              Q_OBJECT
 *              Q_PROTOBUF_OBJECT
 *              Q_PROPERTY(qprotobuf::sint32 prop READ prop WRITE setProp NOTIFY propChanged)
 *              ...
 *              Q_DECLARE_PROTOBUF_SERIALIZERS(MyType)
 *          };
 *          @endcode
 *          Practically code above is generated automaticaly by running qtprotobufgenerator or using cmake build macro
 *          generate_qtprotobuf, based on .proto files. But it's still possible to reuse manually written code if needed.
 */
class QTPROTOBUFSHARED_EXPORT QProtobufSerializerRegistry
{
    QProtobufSerializerRegistry() = delete;
    ~QProtobufSerializerRegistry() = delete;

    Q_DISABLE_COPY(QProtobufSerializerRegistry)
    QProtobufSerializerRegistry(QProtobufSerializerRegistry &&) = delete;
    QProtobufSerializerRegistry &operator =(QProtobufSerializerRegistry &&) = delete;

    static QAbstractProtobufSerializer::SerializerRegistry handlers;
    static std::unique_ptr<QAbstractProtobufSerializer> basicSerializer;
    static QAbstractProtobufSerializer::SerializationHandlers empty;
public:
    static const QAbstractProtobufSerializer::SerializationHandlers &handler(int userType);
    /**
     * @brief Serialization of a registered qtproto message object into byte-array
     *
     *
     * @param[in] object Pointer to QObject containing message to be serialized
     * @result serialized message bytes
     */
    template<typename T>
    static QByteArray serialize(const QObject *object) {
        qProtoDebug() << T::staticMetaObject.className() << "serialize";
        return basicSerializer->serializeObjectCommon(object, T::propertyOrdering, T::staticMetaObject);
    }

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
    static void deserialize(QObject *object, const QByteArray &array) {
        qProtoDebug() << T::staticMetaObject.className() << "deserialize";
        basicSerializer->deserializeObjectCommon(object, array, T::propertyOrdering, T::staticMetaObject);
    }


    /**
     * @brief Registers serializers for type T in QProtobufSerializerRegistry
     *
     * @details generates default serializers for type T. Type T has to be inherited of QObject.
     */
    template<typename T>
    static void registerSerializers() {
        handlers[qMetaTypeId<T *>()] = { serializeComplexType<T>,
                deserializeComplexType<T>, LengthDelimited };
        handlers[qMetaTypeId<QList<QSharedPointer<T>>>()] = { serializeList<T>,
                deserializeList<T>, LengthDelimited };
    }

    /**
     * @brief Registers serializers for type QMap<K, V> in QProtobufSerializerRegistry
     *
     * @details generates default serializers for QMap<K, V>.
     */
    template<typename K, typename V,
             typename std::enable_if_t<!std::is_base_of<QObject, V>::value, int> = 0>
    static void registerMap() {
        handlers[qMetaTypeId<QMap<K, V>>()] = { serializeMap<K, V>,
        deserializeMap<K, V>, LengthDelimited };
    }

    /**
     * @brief Registers serializers for type QMap<K, V> in QProtobufSerializerRegistry
     *
     * @details generates default serializers for QMap<K, V>. Specialization for V type
     *          inherited of QObject.
     */
    template<typename K, typename V,
             typename std::enable_if_t<std::is_base_of<QObject, V>::value, int> = 0>
    static void registerMap() {
        handlers[qMetaTypeId<QMap<K, QSharedPointer<V>>>()] = { serializeMap<K, V>,
        deserializeMap<K, V>, LengthDelimited };
    }

private:
    /**
     * @private
     *
     * @brief default serializer template for type T inherited of QObject
     */
    template <typename T,
              typename std::enable_if_t<std::is_base_of<QObject, T>::value, int> = 0>
    static QByteArray serializeComplexType(const QVariant &value, int &/*outFieldIndex*/) {
        return basicSerializer->serializeObject(value.value<T *>(), T::propertyOrdering, T::staticMetaObject);
    }

    /**
     * @private
     *
     * @brief default serializer template for list of type T objects inherited of QObject
     */
    template<typename V,
             typename std::enable_if_t<std::is_base_of<QObject, V>::value, int> = 0>
    static QByteArray serializeList(const QVariant &listValue, int &outFieldIndex) {
        QList<QSharedPointer<V>> list = listValue.value<QList<QSharedPointer<V>>>();

        qProtoDebug() << __func__ << "listValue.count" << list.count() << "outFiledIndex" << outFieldIndex;

        if (list.count() <= 0) {
            outFieldIndex = NotUsedFieldIndex;
            return QByteArray();
        }

        QByteArray serializedList;
        for (auto &value : list) {
            if (!value) {
                qProtoWarning() << "Null pointer in list";
                continue;
            }
            serializedList.append(basicSerializer->serializeListObject(value.data(), V::propertyOrdering, V::staticMetaObject, outFieldIndex));
        }

        outFieldIndex = NotUsedFieldIndex;

        return serializedList;
    }

    /**
     * @private
     *
     * @brief default serializer template for map of key K, value V
     */
    template<typename K, typename V,
             typename std::enable_if_t<!std::is_base_of<QObject, V>::value, int> = 0>
    static QByteArray serializeMap(const QVariant &value, int &outFieldIndex) {
        QMap<K,V> mapValue = value.value<QMap<K,V>>();
        using ItType = typename QMap<K,V>::const_iterator;
        QByteArray mapResult;

        for ( ItType it = mapValue.constBegin(); it != mapValue.constEnd(); it++) {
            mapResult.append(basicSerializer->serializeMapPair(QVariant::fromValue<K>(it.key()), QVariant::fromValue<V>(it.value()), outFieldIndex));
        }
        outFieldIndex = NotUsedFieldIndex;
        return mapResult;
    }

    /**
     * @private
     *
     * @brief default serializer template for map of type key K, value V. Specialization for V
     *        inherited of QObject
     */
    template<typename K, typename V,
             typename std::enable_if_t<std::is_base_of<QObject, V>::value, int> = 0>
    static QByteArray serializeMap(const QVariant &value, int &outFieldIndex) {
        QMap<K, QSharedPointer<V>> mapValue = value.value<QMap<K, QSharedPointer<V>>>();
        using ItType = typename QMap<K, QSharedPointer<V>>::const_iterator;
        QByteArray mapResult;

        for ( ItType it = mapValue.constBegin(); it != mapValue.constEnd(); it++) {
            if (it.value().isNull()) {
                qProtoWarning() << __func__ << "Trying to serialize map value that contains nullptr";
                continue;
            }
            mapResult.append(basicSerializer->serializeMapPair(QVariant::fromValue<K>(it.key()), QVariant::fromValue<V *>(it.value().data()), outFieldIndex));
        }
        outFieldIndex = NotUsedFieldIndex;
        return mapResult;
    }

    /**
     * @private
     *
     * @brief default deserializer template for type T inherited of QObject
     */
    template <typename T,
              typename std::enable_if_t<std::is_base_of<QObject, T>::value, int> = 0>
    static void deserializeComplexType(SelfcheckIterator &it, QVariant &to) {
        T *value = new T;
        basicSerializer->deserializeObject(value, it, T::propertyOrdering, T::staticMetaObject);
        to = QVariant::fromValue<T *>(value);
    }

    /**
     * @private
     *
     * @brief default deserializer template for list of type T objects inherited of QObject
     */
    template <typename V,
              typename std::enable_if_t<std::is_base_of<QObject, V>::value, int> = 0>
    static void deserializeList(SelfcheckIterator &it, QVariant &previous) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        V *newValue = new V;
        QList<QSharedPointer<V>> list = previous.value<QList<QSharedPointer<V>>>();
        basicSerializer->deserializeListObject(newValue, it, V::propertyOrdering, V::staticMetaObject);
        list.append(QSharedPointer<V>(newValue));
        previous.setValue(list);
    }

    /**
     * @private
     *
     * @brief default deserializer template for map of key K, value V
     */
    template <typename K, typename V,
              typename std::enable_if_t<!std::is_base_of<QObject, V>::value, int> = 0>
    static void deserializeMap(SelfcheckIterator &it, QVariant &previous) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        QMap<K, V> out = previous.value<QMap<K, V>>();
        QVariant key = QVariant::fromValue<K>(K());
        QVariant value = QVariant::fromValue<V>(V());

        basicSerializer->deserializeMapPair(key, value, it);
        out[key.value<K>()] = value.value<V>();
        previous = QVariant::fromValue<QMap<K, V>>(out);
    }

    /**
     * @private
     *
     * @brief default deserializer template for map of type key K, value V. Specialization for V
     *        inherited of QObject
     */
    template <typename K, typename V,
              typename std::enable_if_t<std::is_base_of<QObject, V>::value, int> = 0>
    static void deserializeMap(SelfcheckIterator &it, QVariant &previous) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        auto out = previous.value<QMap<K, QSharedPointer<V>>>();
        QVariant key = QVariant::fromValue<K>(K());
        QVariant value = QVariant::fromValue<V *>(nullptr);

        basicSerializer->deserializeMapPair(key, value, it);
        out[key.value<K>()] = QSharedPointer<V>(value.value<V *>());
        previous = QVariant::fromValue<QMap<K, QSharedPointer<V>>>(out);
    }
};

/** @} */
}
