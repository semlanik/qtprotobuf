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

#pragma once //QQmlListPropertyConstructor

#include <QQmlListProperty>
#include <QQmlEngine>

QT_BEGIN_NAMESPACE

//! \private
template<typename T>
static void qProtobufQmlListPropertyAppend(QQmlListProperty<T> *p, T *v) {
    QQmlEngine::setObjectOwnership(v, QQmlEngine::CppOwnership);
    reinterpret_cast<QList<QSharedPointer<T>> *>(p->data)->append(QSharedPointer<T>(v));
}

//! \private
template<typename T>
static qsizetype qProtobufQmlListPropertyCount(QQmlListProperty<T> *p) {
    return reinterpret_cast<QList<QSharedPointer<T>> *>(p->data)->size();
}

//! \private
template<typename T>
static T *qProtobufQmlListPropertyAt(QQmlListProperty<T> *p, qsizetype index) {
    return reinterpret_cast<QList<QSharedPointer<T>> *>(p->data)->at(index).data();
}

//! \private
template<typename T>
static void qProtobufQmlListPropertyClear(QQmlListProperty<T> *p) {
    reinterpret_cast<QList<QSharedPointer<T>> *>(p->data)->clear();
}

//using ClearFunction = void (*)(QQmlListProperty<T> *);
//using ReplaceFunction = void (*)(QQmlListProperty<T> *, qsizetype, T *);
//using RemoveLastFunction = void (*)(QQmlListProperty<T> *);

//! \private
template<typename T>
static QQmlListProperty<T> qProtobufConstructQmlListProperty(QObject *p, QList<QSharedPointer<T>> *data)
{
    return QQmlListProperty<T>(p, data, qProtobufQmlListPropertyAppend<T>, qProtobufQmlListPropertyCount<T>,
                               qProtobufQmlListPropertyAt<T>, qProtobufQmlListPropertyClear<T>);
}

QT_END_NAMESPACE
