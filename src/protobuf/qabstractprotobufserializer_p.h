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

#include <functional>

#include "qtprotobuftypes.h"
#include "qtprotobuflogging.h"
#include "qtprotobufglobal.h"

namespace QtProtobuf {
    class QAbstractProtobufSerializer;
    class QProtobufSelfcheckIterator;
}

namespace QtProtobufPrivate {
//! \private
constexpr int NotUsedFieldIndex = -1;

using Serializer = std::function<QByteArray(const QtProtobuf::QAbstractProtobufSerializer *, const QVariant &, int)>;
/*!
 * \brief Deserializer is interface function for deserialize method
 */
using Deserializer = std::function<void(const QtProtobuf::QAbstractProtobufSerializer *, QtProtobuf::QProtobufSelfcheckIterator &, QVariant &)>;

/*!
 * \brief SerializationHandlers contains set of objects that required for class serializaion/deserialization
 */
struct SerializationHandler {
    Serializer serializer; /*!< serializer assigned to class */
    Deserializer deserializer;/*!< deserializer assigned to class */
    QtProtobuf::WireTypes type;/*!< Serialization WireType */
};

extern Q_PROTOBUF_EXPORT SerializationHandler &findHandler(int userType);
extern Q_PROTOBUF_EXPORT void registerHandler(int userType, const SerializationHandler &handlers);

/*!
 * \private
 *
 * \brief default serializer template for type T inherited of QObject
 */
template <typename T,
          typename std::enable_if_t<std::is_base_of<QObject, T>::value, int> = 0>
QByteArray serializeObject(const QtProtobuf::QAbstractProtobufSerializer *serializer, const QVariant &value, int outFieldIndex) {
    Q_ASSERT_X(serializer != nullptr, "QAbstractProtobufSerializer", "serializer set is not setup");
    return serializer->serializeObject(value.value<T *>(), T::propertyOrdering, T::staticMetaObject, outFieldIndex);
}

/*!
 * \private
 *
 * \brief default serializer template for list of type T objects inherited of QObject
 */
template<typename V,
         typename std::enable_if_t<std::is_base_of<QObject, V>::value, int> = 0>
QByteArray serializeList(const QtProtobuf::QAbstractProtobufSerializer *serializer, const QVariant &listValue, int outFieldIndex) {
    Q_ASSERT_X(serializer != nullptr, "QAbstractProtobufSerializer", "serializer set is not setup");
    QList<QSharedPointer<V>> list = listValue.value<QList<QSharedPointer<V>>>();

    qProtoDebug() << __func__ << "listValue.count" << list.count() << "outFiledIndex" << outFieldIndex;

    if (list.count() <= 0) {
        return QByteArray();
    }

    QByteArray serializedList;
    for (auto &value : list) {
        if (!value) {
            qProtoWarning() << "Null pointer in list";
            continue;
        }
        serializedList.append(serializer->serializeListObject(value.data(), V::propertyOrdering, V::staticMetaObject, outFieldIndex));
    }
    return serializedList;
}

/*!
 * \private
 *
 * \brief default serializer template for map of key K, value V
 */
template<typename K, typename V,
         typename std::enable_if_t<!std::is_base_of<QObject, V>::value, int> = 0>
QByteArray serializeMap(const QtProtobuf::QAbstractProtobufSerializer *serializer, const QVariant &value, int outFieldIndex) {
    Q_ASSERT_X(serializer != nullptr, "QAbstractProtobufSerializer", "serializer set is not setup");
    QMap<K,V> mapValue = value.value<QMap<K,V>>();
    using ItType = typename QMap<K,V>::const_iterator;
    QByteArray mapResult;

    for ( ItType it = mapValue.constBegin(); it != mapValue.constEnd(); it++) {
        mapResult.append(serializer->serializeMapPair(QVariant::fromValue<K>(it.key()), QVariant::fromValue<V>(it.value()), outFieldIndex));
    }
    return mapResult;
}

/*!
 * \private
 *
 * \brief default serializer template for map of type key K, value V. Specialization for V
 *        inherited of QObject
 */
template<typename K, typename V,
         typename std::enable_if_t<std::is_base_of<QObject, V>::value, int> = 0>
QByteArray serializeMap(const QtProtobuf::QAbstractProtobufSerializer *serializer, const QVariant &value, int outFieldIndex) {
    Q_ASSERT_X(serializer != nullptr, "QAbstractProtobufSerializer", "serializer set is not setup");
    QMap<K, QSharedPointer<V>> mapValue = value.value<QMap<K, QSharedPointer<V>>>();
    using ItType = typename QMap<K, QSharedPointer<V>>::const_iterator;
    QByteArray mapResult;

    for ( ItType it = mapValue.constBegin(); it != mapValue.constEnd(); it++) {
        if (it.value().isNull()) {
            qProtoWarning() << __func__ << "Trying to serialize map value that contains nullptr";
            continue;
        }
        mapResult.append(serializer->serializeMapPair(QVariant::fromValue<K>(it.key()), QVariant::fromValue<V *>(it.value().data()), outFieldIndex));
    }
    return mapResult;
}

/*!
 * \private
 *
 * \brief default deserializer template for type T inherited of QObject
 */
template <typename T,
          typename std::enable_if_t<std::is_base_of<QObject, T>::value, int> = 0>
void deserializeObject(const QtProtobuf::QAbstractProtobufSerializer *serializer, QtProtobuf::QProtobufSelfcheckIterator &it, QVariant &to) {
    Q_ASSERT_X(serializer != nullptr, "QAbstractProtobufSerializer", "serializer set is not setup");
    T *value = new T;
    serializer->deserializeObject(value, it, T::propertyOrdering, T::staticMetaObject);
    to = QVariant::fromValue<T *>(value);
}

/*!
 * \private
 *
 * \brief default deserializer template for list of type T objects inherited of QObject
 */
template <typename V,
          typename std::enable_if_t<std::is_base_of<QObject, V>::value, int> = 0>
void deserializeList(const QtProtobuf::QAbstractProtobufSerializer *serializer, QtProtobuf::QProtobufSelfcheckIterator &it, QVariant &previous) {
    Q_ASSERT_X(serializer != nullptr, "QAbstractProtobufSerializer", "serializer set is not setup");
    qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

    V *newValue = new V;
    QList<QSharedPointer<V>> list = previous.value<QList<QSharedPointer<V>>>();
    serializer->deserializeListObject(newValue, it, V::propertyOrdering, V::staticMetaObject);
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
    Q_ASSERT_X(serializer != nullptr, "QAbstractProtobufSerializer", "serializer set is not setup");
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
    Q_ASSERT_X(serializer != nullptr, "QAbstractProtobufSerializer", "serializer set is not setup");
    qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

    auto out = previous.value<QMap<K, QSharedPointer<V>>>();
    QVariant key = QVariant::fromValue<K>(K());
    QVariant value = QVariant::fromValue<V *>(nullptr);

    serializer->deserializeMapPair(key, value, it);
    out[key.value<K>()] = QSharedPointer<V>(value.value<V *>());
    previous = QVariant::fromValue<QMap<K, QSharedPointer<V>>>(out);
}

}
