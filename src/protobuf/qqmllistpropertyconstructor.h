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

#pragma once

#include <QQmlListProperty>
#include <QQmlEngine>

namespace QtProtobuf {
//! \private
template<typename T>
static void qmllistpropertyAppend(QQmlListProperty<T> *p, T *v) {
    QQmlEngine::setObjectOwnership(v, QQmlEngine::CppOwnership);
    reinterpret_cast<QList<QSharedPointer<T>> *>(p->data)->append(QSharedPointer<T>(v));
}

//! \private
template<typename T>
static int qmllistpropertyCount(QQmlListProperty<T> *p) {
    return reinterpret_cast<QList<QSharedPointer<T>> *>(p->data)->count();
}

//! \private
template<typename T>
static T *qmllistpropertyAt(QQmlListProperty<T> *p, int index) {
    return reinterpret_cast<QList<QSharedPointer<T>> *>(p->data)->at(index).data();
}

//! \private
template<typename T>
static void qmllistpropertyReset(QQmlListProperty<T> *p) {
    reinterpret_cast<QList<QSharedPointer<T>> *>(p->data)->clear();
}

//! \private
template<typename T>
static QQmlListProperty<T> constructQmlListProperty(QObject *p, QList<QSharedPointer<T>> *data)
{
    return QQmlListProperty<T>(p, data, qmllistpropertyAppend<T>, qmllistpropertyCount<T>,
                               qmllistpropertyAt<T>, qmllistpropertyReset<T>);
}

}
