/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>
 *
 * This file is part of QtProtobuf project https://git.semlanik.org/semlanik/qtprotobuf
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
#include <QVariant>
#include <QMetaObject>
#include <QMetaEnum>

#include <functional>

#include "qtprotobuftypes.h"
#include "qtprotobuflogging.h"
#include "qtprotobufglobal.h"

namespace QtProtobuf {
    class QAbstractProtobufSerializer;
    class QProtobufSelfcheckIterator;
    class QProtobufMetaProperty;
}

namespace QtProtobufPrivate {
//! \private
constexpr int NotUsedFieldIndex = -1;

using Serializer = std::function<void(const QtProtobuf::QAbstractProtobufSerializer *, const QVariant &, const QtProtobuf::QProtobufMetaProperty &, QByteArray &)>;
/*!
 * \brief Deserializer is interface function for deserialize method
 */
using Deserializer = std::function<void(const QtProtobuf::QAbstractProtobufSerializer *, QtProtobuf::QProtobufSelfcheckIterator &, QVariant &)>;

enum HandlerType {
    ObjectHandler,
    ListHandler,
    MapHandler
};

/*!
 * \private
 * \brief SerializationHandlers contains set of objects that required for class serializaion/deserialization
 */
struct SerializationHandler {
    Serializer serializer; /*!< serializer assigned to class */
    Deserializer deserializer;/*!< deserializer assigned to class */
    HandlerType type;/*!< Serialization WireType */
};

extern Q_PROTOBUF_EXPORT SerializationHandler &findHandler(int userType);
extern Q_PROTOBUF_EXPORT void registerHandler(int userType, const SerializationHandler &handlers);

/*!
 * \private
 * \brief default serializer template for type T inherited of QObject
 */
template <typename T,
          typename std::enable_if_t<std::is_base_of<QObject, T>::value, int> = 0>
void serializeObject(const QtProtobuf::QAbstractProtobufSerializer *serializer, const QVariant &value, const QtProtobuf::QProtobufMetaProperty &metaProperty, QByteArray &buffer) {
    Q_ASSERT_X(serializer != nullptr, "QAbstractProtobufSerializer", "Serializer is null");
    buffer.append(serializer->serializeObject(value.value<T *>(), T::protobufMetaObject, metaProperty));
}

/*!
 * \private
 * \brief default serializer template for list of type T objects inherited of QObject
 */
template<typename V,
         typename std::enable_if_t<std::is_base_of<QObject, V>::value, int> = 0>
void serializeList(const QtProtobuf::QAbstractProtobufSerializer *serializer, const QVariant &listValue, const QtProtobuf::QProtobufMetaProperty &metaProperty, QByteArray &buffer) {
    Q_ASSERT_X(serializer != nullptr, "QAbstractProtobufSerializer", "Serializer is null");
    QList<QSharedPointer<V>> list = listValue.value<QList<QSharedPointer<V>>>();

    qProtoDebug() << __func__ << "listValue.count" << list.count();

    buffer.append(serializer->serializeListBegin(metaProperty));
    for (auto &value : list) {
        if (!value) {
            qProtoWarning() << "Null pointer in list";
            continue;
        }
        buffer.append(serializer->serializeListObject(value.data(), V::protobufMetaObject, metaProperty));
    }
    buffer.append(serializer->serializeListEnd(buffer, metaProperty));
}

/*!
 * \private
 * \brief default serializer template for map of key K, value V
 */
template<typename K, typename V,
         typename std::enable_if_t<!std::is_base_of<QObject, V>::value, int> = 0>
void serializeMap(const QtProtobuf::QAbstractProtobufSerializer *serializer, const QVariant &value, const QtProtobuf::QProtobufMetaProperty &metaProperty, QByteArray &buffer) {
    Q_ASSERT_X(serializer != nullptr, "QAbstractProtobufSerializer", "Serializer is null");
    QMap<K,V> mapValue = value.value<QMap<K,V>>();
    buffer.append(serializer->serializeMapBegin(metaProperty));
    for (auto it = mapValue.constBegin(); it != mapValue.constEnd(); it++) {
        buffer.append(serializer->serializeMapPair(QVariant::fromValue<K>(it.key()), QVariant::fromValue<V>(it.value()), metaProperty));
    }
    buffer.append(serializer->serializeMapEnd(buffer, metaProperty));
}

/*!
 * \private
 * \brief default serializer template for map of type key K, value V. Specialization for V inherited of QObject
 */
template<typename K, typename V,
         typename std::enable_if_t<std::is_base_of<QObject, V>::value, int> = 0>
void serializeMap(const QtProtobuf::QAbstractProtobufSerializer *serializer, const QVariant &value, const QtProtobuf::QProtobufMetaProperty &metaProperty, QByteArray &buffer) {
    Q_ASSERT_X(serializer != nullptr, "QAbstractProtobufSerializer", "Serializer is null");
    QMap<K, QSharedPointer<V>> mapValue = value.value<QMap<K, QSharedPointer<V>>>();
    buffer.append(serializer->serializeMapBegin(metaProperty));
    for (auto it = mapValue.constBegin(); it != mapValue.constEnd(); it++) {
        if (it.value().isNull()) {
            qProtoWarning() << __func__ << "Trying to serialize map value that contains nullptr";
            continue;
        }
        buffer.append(serializer->serializeMapPair(QVariant::fromValue<K>(it.key()), QVariant::fromValue<V *>(it.value().data()), metaProperty));
    }
    buffer.append(serializer->serializeMapEnd(buffer, metaProperty));
}

/*!
 * \private
 * \brief default serializer template for enum types
 */
template<typename T,
         typename std::enable_if_t<std::is_enum<T>::value, int> = 0>
void serializeEnum(const QtProtobuf::QAbstractProtobufSerializer *serializer, const QVariant &value, const QtProtobuf::QProtobufMetaProperty &metaProperty, QByteArray &buffer) {
    Q_ASSERT_X(serializer != nullptr, "QAbstractProtobufSerializer", "Serializer is null");
    buffer.append(serializer->serializeEnum(QtProtobuf::int64(value.value<T>()), QMetaEnum::fromType<T>(),  metaProperty));
}

/*!
 * \private
 * \brief default serializer template for enum list types
 */
template<typename T,
         typename std::enable_if_t<std::is_enum<T>::value, int> = 0>
void serializeEnumList(const QtProtobuf::QAbstractProtobufSerializer *serializer, const QVariant &value, const QtProtobuf::QProtobufMetaProperty &metaProperty, QByteArray &buffer) {
    Q_ASSERT_X(serializer != nullptr, "QAbstractProtobufSerializer", "Serializer is null");
    QList<QtProtobuf::int64> intList;
    for (auto enumValue : value.value<QList<T>>()) {
        intList.append(QtProtobuf::int64(enumValue));
    }
    buffer.append(serializer->serializeEnumList(intList, QMetaEnum::fromType<T>(), metaProperty));
}

/*!
 * \private
 * \brief default deserializer template for type T inherited of QObject
 */
template <typename T,
          typename std::enable_if_t<std::is_base_of<QObject, T>::value, int> = 0>
void deserializeObject(const QtProtobuf::QAbstractProtobufSerializer *serializer, QtProtobuf::QProtobufSelfcheckIterator &it, QVariant &to) {
    Q_ASSERT_X(serializer != nullptr, "QAbstractProtobufSerializer", "Serializer is null");
    T *value = new T;
    serializer->deserializeObject(value, T::protobufMetaObject, it);
    to = QVariant::fromValue<T *>(value);
}

/*!
 * \private
 * \brief default deserializer template for list of type T objects inherited of QObject
 */
template <typename V,
          typename std::enable_if_t<std::is_base_of<QObject, V>::value, int> = 0>
void deserializeList(const QtProtobuf::QAbstractProtobufSerializer *serializer, QtProtobuf::QProtobufSelfcheckIterator &it, QVariant &previous) {
    Q_ASSERT_X(serializer != nullptr, "QAbstractProtobufSerializer", "Serializer is null");
    qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

    V *newValue = new V;
    QList<QSharedPointer<V>> list = previous.value<QList<QSharedPointer<V>>>();
    serializer->deserializeListObject(newValue, V::protobufMetaObject, it);
    list.append(QSharedPointer<V>(newValue));
    previous.setValue(list);
}

/*!
 * \private
 *
 * \brief default deserializer template for map of key K, value V
 */
template <typename K, typename V,
          typename std::enable_if_t<!std::is_base_of<QObject, V>::value, int> = 0>
void deserializeMap(const QtProtobuf::QAbstractProtobufSerializer *serializer, QtProtobuf::QProtobufSelfcheckIterator &it, QVariant &previous) {
    Q_ASSERT_X(serializer != nullptr, "QAbstractProtobufSerializer", "Serializer is null");
    qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

    QMap<K, V> out = previous.value<QMap<K, V>>();
    QVariant key = QVariant::fromValue<K>(K());
    QVariant value = QVariant::fromValue<V>(V());

    serializer->deserializeMapPair(key, value, it);
    out[key.value<K>()] = value.value<V>();
    previous = QVariant::fromValue<QMap<K, V>>(out);
}

/*!
 * \private
 *
 * \brief default deserializer template for map of type key K, value V. Specialization for V
 *        inherited of QObject
 */
template <typename K, typename V,
          typename std::enable_if_t<std::is_base_of<QObject, V>::value, int> = 0>
void deserializeMap(const QtProtobuf::QAbstractProtobufSerializer *serializer, QtProtobuf::QProtobufSelfcheckIterator &it, QVariant &previous) {
    Q_ASSERT_X(serializer != nullptr, "QAbstractProtobufSerializer", "Serializer is null");
    qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

    auto out = previous.value<QMap<K, QSharedPointer<V>>>();
    QVariant key = QVariant::fromValue<K>(K());
    QVariant value = QVariant::fromValue<V *>(nullptr);

    serializer->deserializeMapPair(key, value, it);
    out[key.value<K>()] = QSharedPointer<V>(value.value<V *>());
    previous = QVariant::fromValue<QMap<K, QSharedPointer<V>>>(out);
}

/*!
 * \private
 *
 * \brief default deserializer template for enum type T
 */
template <typename T,
          typename std::enable_if_t<std::is_enum<T>::value, int> = 0>
void deserializeEnum(const QtProtobuf::QAbstractProtobufSerializer *serializer, QtProtobuf::QProtobufSelfcheckIterator &it, QVariant &to) {
    Q_ASSERT_X(serializer != nullptr, "QAbstractProtobufSerializer", "Serializer is null");
    QtProtobuf::int64 intValue;
    serializer->deserializeEnum(intValue, QMetaEnum::fromType<T>(), it);
    to = QVariant::fromValue<T>(static_cast<T>(intValue._t));
}

/*!
 * \private
 *
 * \brief default deserializer template for enumList type T
 */
template <typename T,
          typename std::enable_if_t<std::is_enum<T>::value, int> = 0>
void deserializeEnumList(const QtProtobuf::QAbstractProtobufSerializer *serializer, QtProtobuf::QProtobufSelfcheckIterator &it, QVariant &previous) {
    Q_ASSERT_X(serializer != nullptr, "QAbstractProtobufSerializer", "Serializer is null");
    QList<QtProtobuf::int64> intList;
    serializer->deserializeEnumList(intList, QMetaEnum::fromType<T>(), it);
    QList<T> enumList = previous.value<QList<T>>();
    for (auto intValue : intList) {
        enumList.append(static_cast<T>(intValue._t));
    }
    previous = QVariant::fromValue<QList<T>>(enumList);
}
}
