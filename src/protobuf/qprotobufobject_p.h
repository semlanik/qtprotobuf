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

    static QByteArray serializeProperty(const QVariant &propertyValue, int fieldIndex, bool isEnum);
    static void deserializeProperty(QObject *object, SelfcheckIterator &it, const std::unordered_map<int, int> &propertyOrdering, const QMetaObject &metaObject);

    static QByteArray serializeObject(const QObject *object, const std::unordered_map<int, int> &propertyOrdering, const QMetaObject &metaObject);
    static void deserializeObject(QObject *object, SelfcheckIterator &it, const std::unordered_map<int, int> &propertyOrdering, const QMetaObject &metaObject);

    static QByteArray serializeListObject(const QObject *object, const std::unordered_map<int, int> &propertyOrdering, const QMetaObject &metaObject, int fieldIndex);

    static QByteArray serializeMapPair(const QVariant &key, const QVariant &value, int fieldIndex);
    static void deserializeMapPair(QVariant &key, QVariant &value, SelfcheckIterator &it);

    static QByteArray serializeObjectCommon(const QObject *object, const std::unordered_map<int, int> &propertyOrdering, const QMetaObject &metaObject);//TODO make protected
    static void deserializeObjectCommon(QObject *object, const QByteArray &array, const std::unordered_map<int, int> &propertyOrdering, const QMetaObject &metaObject);//TODO make protected
    //###########################################################################
    //                           Serialization helpers
    //###########################################################################
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
            serializedList.append(serializeListObject(value.data(), V::propertyOrdering, V::staticMetaObject, outFieldIndex));
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

        for ( ItType it = mapValue.constBegin(); it != mapValue.constEnd(); it++) {
            mapResult.append(serializeMapPair(QVariant::fromValue<K>(it.key()), QVariant::fromValue<V>(it.value()), outFieldIndex));
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

        for ( ItType it = mapValue.constBegin(); it != mapValue.constEnd(); it++) {
            if (it.value().isNull()) {
                qProtoWarning() << __func__ << "Trying to serialize map value that contains nullptr";
                continue;
            }
            mapResult.append(serializeMapPair(QVariant::fromValue<K>(it.key()), QVariant::fromValue<V *>(it.value().data()), outFieldIndex));
        }
        outFieldIndex = NotUsedFieldIndex;
        return mapResult;
    }

    //###########################################################################
    //                           Deserialization helpers
    //###########################################################################
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

        QMap<K, V> out = previous.value<QMap<K, V>>();
        QVariant key = QVariant::fromValue<K>(K());
        QVariant value = QVariant::fromValue<V>(V());

        deserializeMapPair(key, value, it);
        out[key.value<K>()] = value.value<V>();
        previous = QVariant::fromValue<QMap<K, V>>(out);
    }

    template <typename K, typename V,
              typename std::enable_if_t<std::is_base_of<QObject, V>::value, int> = 0>
    static void deserializeMap(SelfcheckIterator &it, QVariant &previous) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        auto out = previous.value<QMap<K, QSharedPointer<V>>>();
        QVariant key = QVariant::fromValue<K>(K());
        QVariant value = QVariant::fromValue<V *>(nullptr);

        deserializeMapPair(key, value, it);
        out[key.value<K>()] = QSharedPointer<V>(value.value<V *>());
        previous = QVariant::fromValue<QMap<K, QSharedPointer<V>>>(out);
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

    static void deserializeMapField(QVariant &value, SelfcheckIterator &it) {
        auto serializer = serializers.at(value.userType());//Throws exception if not found
        serializer.deserializer(it, value);
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
        return ProtobufObjectPrivate::serializeObject(value.value<T *>(), T::propertyOrdering, T::staticMetaObject);
    }

    template <typename T,
              typename std::enable_if_t<std::is_base_of<QObject, T>::value, int> = 0>
    static void deserializeComplexType(SelfcheckIterator &it, QVariant &to) {
        T *value = new T;
        deserializeObject(value, it, T::propertyOrdering, T::staticMetaObject);
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
        return serializeObjectCommon(object, T::propertyOrdering, T::staticMetaObject);
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
    static void deserializeObject(QObject *object, const QByteArray &array) {
        qProtoDebug() << T::staticMetaObject.className() << "deserialize";
        deserializeObjectCommon(object, array, T::propertyOrdering, T::staticMetaObject);
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

}
