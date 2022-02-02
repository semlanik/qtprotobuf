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

#pragma once //QAbstractProtobufSerializer

#include <QObject>
#include <QVariant>
#include <QMetaObject>

#include <unordered_map>
#include <functional>
#include <memory>

#include <QtProtobuf/qtprotobuftypes.h>
#include <QtProtobuf/qprotobufselfcheckiterator.h>

#include <QtProtobuf/qtprotobufglobal.h>

QT_BEGIN_NAMESPACE

class QProtobufMetaProperty;
class QProtobufMetaObject;

/*!
 * \ingroup QtProtobuf
 * \brief The QAbstractProtobufSerializer class is interface that represents basic functions for serialization/deserialization
 *
 * \details The QAbstractProtobufSerializer class registers serializers/deserializers for classes inherited of QObject.
 *          To register serializers for user-defined class it has to be inherited of QObject and contains
 *          Q_DECLARE_PROTOBUF_SERIALIZERS macro's.
 *          \code{.cpp}
 *          class MyType : public QObject
 *          {
 *              Q_OBJECT
 *              Q_PROTOBUF_OBJECT
 *              Q_PROPERTY(qprotobuf::sint32 prop READ prop WRITE setProp NOTIFY propChanged)
 *              ...
 *              Q_DECLARE_PROTOBUF_SERIALIZERS(MyType)
 *          };
 *          \endcode
 *          Practically code above is generated automaticaly by running qtprotobufgenerator or using cmake build macro
 *          qt6_protobuf_generate, based on .proto files. But it's still possible to reuse manually written code if needed.
 *
 *          This class should be used as base for specific serializers. The handlers property contains all
 *          message-specific serializers and should be used while serialization/deserialization. Inherited classes should reimplement
 *          scope of virtual methods that used by registred message serialization/deserialization functions.
 */
class Q_PROTOBUF_EXPORT QAbstractProtobufSerializer
{
public:
    /*!
     * \brief Serialization of a registered qtproto message object into byte-array
     *
     *
     * \param[in] object Pointer to QObject containing message to be serialized
     * \result serialized message bytes
     */
    template<typename T>
    QByteArray serialize(const QObject *object) {
        Q_ASSERT(object != nullptr);
        return serializeMessage(object, T::protobufMetaObject);
    }

    /*!
     * \brief Deserialization of a byte-array into a registered qtproto message object
     *
     * \details Properties in a message are identified via ProtobufObjectPrivate::decodeHeader.
     *          Bytes corresponding to unexpected properties are skipped without any exception
     *
     * \param[out] object Pointer to memory where result of deserialization should be injected
     * \param[in] data Bytes with serialized message
     */
    template<typename T>
    void deserialize(T *object, const QByteArray &data) {
        Q_ASSERT(object != nullptr);
        //Initialize default object first and make copy aferwards, it's necessary to set default
        //values of properties that was not stored in data.
        T newValue;
        QT_TRY {
            deserializeMessage(&newValue, T::protobufMetaObject, data);
        } QT_CATCH(...) {
            *object = newValue;
            QT_RETHROW;
        }
        *object = newValue;
    }

    virtual ~QAbstractProtobufSerializer() = default;

    /*!
     * \brief serializeMessage
     * \param object
     * \param propertyOrdering
     * \param metaObject
     * \return
     */
    virtual QByteArray serializeMessage(const QObject *object, const QProtobufMetaObject &metaObject) const = 0;

    /*!
     * \brief serializeMessage
     * \param object
     * \param propertyOrdering
     * \param metaObject
     * \return
     */
    virtual void deserializeMessage(QObject *object, const QProtobufMetaObject &metaObject, const QByteArray &data) const = 0;

    /*!
     * \brief serializeObject Serializes complete \a object according given \a propertyOrdering and \a metaObject
     *        information
     * \param[in] object Pointer to object to be serialized
     * \param[in] metaObject Protobuf meta object information for given \a object
     * \param[in] metaProperty Information about property to be serialized
     * \return Raw serialized data represented as byte array
     */
    virtual QByteArray serializeObject(const QObject *object, const QProtobufMetaObject &metaObject, const QProtobufMetaProperty &metaProperty) const = 0;

    /*!
     * \brief deserializeObject Deserializes buffer to an \a object
     * \param[out] object Pointer to pre-allocated object
     * \param[in] metaObject Protobuf meta object information for given \a object. Static meta object usualy is used to get actual
     *        property value and write new property to \a object
     * \param[in] it Pointer to beging of buffer where object serialized data is located
     * \param[in] propertyOrdering Ordering of properties for given \a object
     * \param[in] metaProperty Information about property to be serialized
     */
    virtual void deserializeObject(QObject *object, const QProtobufMetaObject &metaObject, QProtobufSelfcheckIterator &it) const = 0;

    /*!
     * \brief serializeListBegin Method called at the begining of object list serialization
     * \param[in] metaProperty Information about property to be serialized
     * \return Raw serialized data represented as byte array
     */
    virtual QByteArray serializeListBegin(const QProtobufMetaProperty &metaProperty) const {
        Q_UNUSED(metaProperty);
        return {};
    }

    /*!
     * \brief serializeListObject Method called to serialize \a object as a part of list property
     * \param[in] object Pointer to object that will be serialized
     * \param[in] metaObject Protobuf meta object information for given \a object
     * \param[in] metaProperty Information about property to be serialized
     * \return Raw serialized data represented as byte array
     */
    virtual QByteArray serializeListObject(const QObject *object, const QProtobufMetaObject &metaObject, const QProtobufMetaProperty &metaProperty) const = 0;

    /*!
     * \brief serializeListEnd Method called at the end of object list serialization
     * \param[in] buffer Buffer at and of list serialization
     * \param[in] metaProperty Information about property to be serialized
     * \return Raw serialized data represented as byte array
     */
    virtual QByteArray serializeListEnd(QByteArray &buffer, const QProtobufMetaProperty &metaProperty) const {
        Q_UNUSED(metaProperty);
        Q_UNUSED(buffer);
        return {};
    }

    /*!
     * \brief deserializeListObject Deserializes an \a object from byte stream as part of list property
     * \param[out] object Pointer to pre-allocated object, that will be appended to list property
     * \param[in] Protobuf meta object information for given \a object. Static meta object usualy is used to get actual
     *        property value and write new property to \a object
     * \param[in] it Pointer to beging of buffer where object serialized data is located
     */
    virtual bool deserializeListObject(QObject *object, const QProtobufMetaObject &metaObject, QProtobufSelfcheckIterator &it) const = 0;

    /*!
     * \brief serializeMapEnd Method called at the begining of map serialization
     * \param[in] metaProperty Information about property to be serialized
     * \return Raw serialized data represented as byte array
     */
    virtual QByteArray serializeMapBegin(const QProtobufMetaProperty &metaProperty) const {
        Q_UNUSED(metaProperty);
        return {};
    }

    /*!
     * \brief serializeMapPair Serializes QMap pair of \a key and \a value to raw data buffer
     * \param[in] key Map key
     * \param[in] value Map value for given \a key
     * \param[in] metaProperty Information about property to be serialized
     * \return Raw serialized data represented as byte array
     *
     * \see https://developers.google.com/protocol-buffers/docs/proto3#maps for details
     */
    virtual QByteArray serializeMapPair(const QVariant &key, const QVariant &value, const QProtobufMetaProperty &metaProperty) const = 0;

    /*!
     * \brief serializeMapEnd Method called at the end of map serialization
     * \param[in] buffer Buffer at and of list serialization
     * \param[in] metaProperty Information about property to be serialized
     * \return Raw serialized data represented as byte array
     */
    virtual QByteArray serializeMapEnd(QByteArray &buffer, const QProtobufMetaProperty &metaProperty) const {
        Q_UNUSED(metaProperty);
        Q_UNUSED(buffer);
        return {};
    }

    /*!
     * \brief deserializeMapPair Deserializes QMap pair of \a key and \a value from raw data
     * \param[out] key Buffer that will be used to store deserialized key. When passed to function, QVariant
     *        already stores default constructed value. So it's possible to receive meta information about type from it.
     * \param[out] value Buffer that will be used to store deserialized value. When passed to function, QVariant
     *        already stores default constructed value. So it's possible to receive meta information about type from it.
     * \param[in] it Points to serialized raw key/value data
     *
     * \see https://developers.google.com/protocol-buffers/docs/proto3#maps for details
     */
    virtual bool deserializeMapPair(QVariant &key, QVariant &value, QProtobufSelfcheckIterator &it) const = 0;

    /*!
     * \brief serializeEnum Serializes enum value represented as int64 type
     * \param[in] value Enum value to be serialized
     * \param[in] metaEnum Information about enumeration type
     * \param[in] metaProperty Information about property to be serialized
     * \return Raw serialized data represented as byte array
     *
     * \see https://developers.google.com/protocol-buffers/docs/proto3#maps for details
     */
    virtual QByteArray serializeEnum(QtProtobuf::int64 value, const QMetaEnum &metaEnum, const QProtobufMetaProperty &metaProperty) const = 0;

    /*!
     * \brief serializeEnumList  Method called to serialize list of enum values
     * \param[in] value List of enum values to be serialized, represented as int64
     * \param[in] metaEnum Information about enumeration type
     * \param[in] metaProperty Information about property to be serialized
     * \return Raw serialized data represented as byte array
     *
     * \see https://developers.google.com/protocol-buffers/docs/proto3#maps for details
     */
    virtual QByteArray serializeEnumList(const QList<QtProtobuf::int64> &value, const QMetaEnum &metaEnum, const QProtobufMetaProperty &metaProperty) const = 0;

    /*!
     * \brief deserializeEnum Deserializes enum value from byte stream
     * \param[out] value Buffer that will be used to collect new enum value
     * \param[in] metaEnum Information about enumeration type
     * \param[in] it Points to serialized raw key/value data
     */
    virtual void deserializeEnum(QtProtobuf::int64 &value, const QMetaEnum &metaEnum, QProtobufSelfcheckIterator &it) const = 0;

    /*!
     * \brief deserializeEnum Deserializes list of enum values from byte stream
     * \param[out] value QList that will be used to collect deserialized enum values
     * \param[in] metaEnum Information about enumeration type
     * \param[in] it Points to serialized raw key/value data
     */
    virtual void deserializeEnumList(QList<QtProtobuf::int64> &value, const QMetaEnum &metaEnum, QProtobufSelfcheckIterator &it) const = 0;
};
/*! \} */

QT_END_NAMESPACE

#include <QtProtobuf/qabstractprotobufserializercommon.h>

/*!
 * \brief Registers serializers for type T in QtProtobuf global serializers registry
 * \private
 * \details generates default serializers for type T. Type T has to be inherited of QObject.
 */
template<typename T>
static void qRegisterProtobufType() {
    T::registerTypes();
    QtProtobufPrivate::registerHandler(qMetaTypeId<T *>(), { QtProtobufPrivate::serializeObject<T>,
            QtProtobufPrivate::deserializeObject<T>, QtProtobufPrivate::ObjectHandler });
    QtProtobufPrivate::registerHandler(qMetaTypeId<QList<QSharedPointer<T>>>(), { QtProtobufPrivate::serializeList<T>,
            QtProtobufPrivate::deserializeList<T>, QtProtobufPrivate::ListHandler });
}

/*!
 * \brief Registers serializers for type QMap<K, V> in QtProtobuf global serializers registry
 * \private
 * \details generates default serializers for QMap<K, V>.
 */
template<typename K, typename V,
         typename std::enable_if_t<!std::is_base_of<QObject, V>::value, int> = 0>
inline void qRegisterProtobufMapType() {
    QtProtobufPrivate::registerHandler(qMetaTypeId<QMap<K, V>>(), { QtProtobufPrivate::serializeMap<K, V>,
    QtProtobufPrivate::deserializeMap<K, V>, QtProtobufPrivate::MapHandler });
}

/*!
 * \brief Registers serializers for type QMap<K, V> in QtProtobuf global serializers registry
 * \private
 * \details generates default serializers for QMap<K, V>. Specialization for V type
 *          inherited of QObject.
 */
template<typename K, typename V,
         typename std::enable_if_t<std::is_base_of<QObject, V>::value, int> = 0>
inline void qRegisterProtobufMapType() {
    QtProtobufPrivate::registerHandler(qMetaTypeId<QMap<K, QSharedPointer<V>>>(), { QtProtobufPrivate::serializeMap<K, V>,
    QtProtobufPrivate::deserializeMap<K, V>, QtProtobufPrivate::MapHandler });
}


/*!
 * \brief Registers serializers for enumeration type in QtProtobuf global serializers registry
 * \private
 * \details generates default serializers for enumeration and QList of enumerations.
 */
template<typename T,
         typename std::enable_if_t<std::is_enum<T>::value, int> = 0>
inline void qRegisterProtobufEnumType() {
    QtProtobufPrivate::registerHandler(qMetaTypeId<T>(), { QtProtobufPrivate::serializeEnum<T>,
                                                           QtProtobufPrivate::deserializeEnum<T>, QtProtobufPrivate::ObjectHandler });
    QtProtobufPrivate::registerHandler(qMetaTypeId<QList<T>>(), { QtProtobufPrivate::serializeEnumList<T>,
                                                           QtProtobufPrivate::deserializeEnumList<T>, QtProtobufPrivate::ListHandler });
}
