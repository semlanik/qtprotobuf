/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>, Viktor Kopp <vifactor@gmail.com>
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

#include "qprotobufserializerimpl.h"
#include "qprotobufmetaproperty.h"
#include "qprotobufmetaobject.h"

QProtobufSerializerImpl::QProtobufSerializerImpl()
{
}

QProtobufSerializerImpl::~QProtobufSerializerImpl()
{
}

QByteArray QProtobufSerializerImpl::serializeMessage(const QObject *object,
                                                     const QtProtobuf::QProtobufMetaObject &metaObject) const
{
    Q_UNUSED(object)
    Q_UNUSED(metaObject)
    return QByteArray();
}

void QProtobufSerializerImpl::deserializeMessage(QObject *object,
                                                 const QtProtobuf::QProtobufMetaObject &metaObject,
                                                 const QByteArray &data) const
{
    Q_UNUSED(object)
    Q_UNUSED(data)
    Q_UNUSED(metaObject)
}

QByteArray QProtobufSerializerImpl::serializeObject(const QObject *object,
                                                    const QtProtobuf::QProtobufMetaObject &metaObject,
                                                    const QtProtobuf::QProtobufMetaProperty &metaProperty) const
{
    Q_UNUSED(object)
    Q_UNUSED(metaProperty)
    Q_UNUSED(metaObject)
    return QByteArray();
}

void QProtobufSerializerImpl::deserializeObject(QObject *object,
                                                const QtProtobuf::QProtobufMetaObject &metaObject,
                                                QtProtobuf::QProtobufSelfcheckIterator &it) const
{
    Q_UNUSED(object)
    Q_UNUSED(it)
    Q_UNUSED(metaObject)
}

QByteArray QProtobufSerializerImpl::serializeListObject(const QObject *object,
                                                        const QtProtobuf::QProtobufMetaObject &metaObject,
                                                        const QtProtobuf::QProtobufMetaProperty &metaProperty) const
{
    return serializeObject(object, metaObject, metaProperty);
}

bool QProtobufSerializerImpl::deserializeListObject(QObject *object,
                                                    const QtProtobuf::QProtobufMetaObject &metaObject,
                                                    QtProtobuf::QProtobufSelfcheckIterator &it) const
{
    deserializeObject(object, metaObject, it);
    return true;
}

QByteArray QProtobufSerializerImpl::serializeMapPair(const QVariant &key,
                                                     const QVariant &value,
                                                     const QtProtobuf::QProtobufMetaProperty &metaProperty) const
{
    Q_UNUSED(value)
    Q_UNUSED(metaProperty)
    Q_UNUSED(key)
    return QByteArray();
}

bool QProtobufSerializerImpl::deserializeMapPair(QVariant &key,
                                                 QVariant &value,
                                                 QtProtobuf::QProtobufSelfcheckIterator &it) const
{
    Q_UNUSED(value)
    Q_UNUSED(it)
    Q_UNUSED(key)
    return true;
}

QByteArray QProtobufSerializerImpl::serializeEnum(QtProtobuf::int64 value,
                                                  const QMetaEnum &/*metaEnum*/,
                                                  const QtProtobuf::QProtobufMetaProperty &metaProperty) const
{
    Q_UNUSED(value)
    Q_UNUSED(metaProperty)
    return QByteArray();
}

QByteArray QProtobufSerializerImpl::serializeEnumList(const QList<QtProtobuf::int64> &value,
                                                      const QMetaEnum &/*metaEnum*/,
                                                      const QtProtobuf::QProtobufMetaProperty &metaProperty) const
{
    Q_UNUSED(value)
    Q_UNUSED(metaProperty)
    return QByteArray();
}

void QProtobufSerializerImpl::deserializeEnum(QtProtobuf::int64 &value,
                                              const QMetaEnum &/*metaEnum*/,
                                              QtProtobuf::QProtobufSelfcheckIterator &it) const
{
    Q_UNUSED(value)
    Q_UNUSED(it)
}

void QProtobufSerializerImpl::deserializeEnumList(QList<QtProtobuf::int64> &value,
                                                  const QMetaEnum &/*metaEnum*/,
                                                  QtProtobuf::QProtobufSelfcheckIterator &it) const
{
    Q_UNUSED(value)
    Q_UNUSED(it)
}
