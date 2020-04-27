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

#include "qprotobufjsonserializerimpl.h"
#include "qprotobufmetaobject.h"
#include "qprotobufmetaproperty.h"
#include <QMetaProperty>

QProtobufJsonSerializerImpl::QProtobufJsonSerializerImpl()
{
}

QByteArray QProtobufJsonSerializerImpl::serializeMessage(const QObject *object,
                                                         const QtProtobuf::QProtobufMetaObject &metaObject) const
{
    Q_UNUSED(object)
    Q_UNUSED(metaObject)
    return QByteArray();
}

void QProtobufJsonSerializerImpl::deserializeMessage(QObject *object, const QtProtobuf::QProtobufMetaObject &metaObject,
                                                     const QByteArray &data) const
{
    Q_UNUSED(object)
    Q_UNUSED(data)
    Q_UNUSED(metaObject)
}

QByteArray QProtobufJsonSerializerImpl::serializeObject(const QObject *object,
                                                        const QtProtobuf::QProtobufMetaObject &metaObject,
                                                        const QtProtobuf::QProtobufMetaProperty &/*metaProperty*/) const
{
    Q_UNUSED(object)
    Q_UNUSED(metaObject)
    return QByteArray();
}

void QProtobufJsonSerializerImpl::deserializeObject(QObject *object,
                                                    const QtProtobuf::QProtobufMetaObject &metaObject,
                                                    QtProtobuf::QProtobufSelfcheckIterator &it) const
{
    Q_UNUSED(object)
    Q_UNUSED(it)
    Q_UNUSED(metaObject)
}

QByteArray QProtobufJsonSerializerImpl::serializeListObject(const QObject *object,
                                                            const QtProtobuf::QProtobufMetaObject &metaObject,
                                                            const QtProtobuf::QProtobufMetaProperty &metaProperty) const
{
    Q_UNUSED(object)
    Q_UNUSED(metaObject)
    Q_UNUSED(metaProperty)
    return QByteArray();
}

bool QProtobufJsonSerializerImpl::deserializeListObject(QObject *object,
                                                        const QtProtobuf::QProtobufMetaObject &metaObject,
                                                        QtProtobuf::QProtobufSelfcheckIterator &it) const
{
    Q_UNUSED(object)
    Q_UNUSED(it)
    Q_UNUSED(metaObject)
    return true;
}

QByteArray QProtobufJsonSerializerImpl::serializeMapPair(const QVariant &key, const QVariant &value,
                                                         const QtProtobuf::QProtobufMetaProperty &metaProperty) const
{
    Q_UNUSED(key)
    Q_UNUSED(value)
    Q_UNUSED(metaProperty)
    return QByteArray();
}

bool QProtobufJsonSerializerImpl::deserializeMapPair(QVariant &key, QVariant &value,
                                                     QtProtobuf::QProtobufSelfcheckIterator &it) const
{
    Q_UNUSED(key)
    Q_UNUSED(value)
    Q_UNUSED(it)
    return true;
}

QByteArray QProtobufJsonSerializerImpl::serializeEnum(QtProtobuf::int64 value,
                                                      const QMetaEnum &metaEnum,
                                                      const QtProtobuf::QProtobufMetaProperty &metaProperty) const
{
    Q_UNUSED(value)
    Q_UNUSED(metaEnum)
    Q_UNUSED(metaProperty)
    return QByteArray();
}

QByteArray QProtobufJsonSerializerImpl::serializeEnumList(const QList<QtProtobuf::int64> &value,
                                                          const QMetaEnum &metaEnum,
                                                          const QtProtobuf::QProtobufMetaProperty &metaProperty) const
{
    Q_UNUSED(value)
    Q_UNUSED(metaEnum)
    Q_UNUSED(metaProperty)
    return QByteArray();
}

void QProtobufJsonSerializerImpl::deserializeEnum(QtProtobuf::int64 &value,
                                                  const QMetaEnum &metaEnum,
                                                  QtProtobuf::QProtobufSelfcheckIterator &it) const
{
    Q_UNUSED(value)
    Q_UNUSED(metaEnum)
    Q_UNUSED(it)
}

void QProtobufJsonSerializerImpl::deserializeEnumList(QList<QtProtobuf::int64> &value,
                                                      const QMetaEnum &metaEnum,
                                                      QtProtobuf::QProtobufSelfcheckIterator &it) const
{
    Q_UNUSED(value)
    Q_UNUSED(metaEnum)
    Q_UNUSED(it)
}
