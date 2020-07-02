 /*
 * MIT License
 *
 * Copyright (c) 2020 Alexey Edelev <semlanik@gmail.com>
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

#include <QUrl>
#include <QChar>
#include <QUuid>
#include <QColor>
#include <QTime>
#include <QDate>
#include <QTimeZone>
#include <QDateTime>
#include <QDataStream>
#include <QSize>
#include <QSizeF>
#include <QPoint>
#include <QPointF>
#include <QRect>
#include <QRectF>
#include <QPolygon>
#include <QPolygonF>

#include <qtprotobuftypes.h>
#include <qtprotobufqttypes.h>

#include "qabstractprotobufserializer.h"
#include "qabstractprotobufserializer_p.h"

#include "QtProtobuf/QtCore.qpb.h"
#include "QtProtobuf/QtGui.qpb.h"

namespace QtProtobuf {

::QUrl convert(const ::QtProtobuf::QUrl &from) {
    return ::QUrl(from.url());
}

::QtProtobuf::QUrl convert(const ::QUrl &from) {
    return ::QtProtobuf::QUrl(from.url());
}

::QChar convert(const ::QtProtobuf::QChar &from) {
    QDataStream stream(from.character());
    ::QChar ret;
    stream >> ret;
    return ret;
}

::QtProtobuf::QChar convert(const ::QChar &from) {
    QByteArray out;
    QDataStream stream(&out, QIODevice::WriteOnly);
    stream << from;
    return ::QtProtobuf::QChar(out);
}

::QUuid convert(const ::QtProtobuf::QUuid &from) {
    return ::QUuid(from.uuid());
}

::QtProtobuf::QUuid convert(const ::QUuid &from) {
    return ::QtProtobuf::QUuid(from.toString());
}

::QColor convert(const ::QtProtobuf::QColor &from) {
    return ::QColor(from.red(), from.green(), from.blue(), from.alpha());
}

::QtProtobuf::QColor convert(const ::QColor &from) {
    return ::QtProtobuf::QColor(from.red(), from.green(), from.blue(), from.alpha());
}

::QTime convert(const ::QtProtobuf::QTime &from) {
    return ::QTime(from.hour(), from.minute(), from.second(), from.msec());
}

::QtProtobuf::QTime convert(const ::QTime &from) {
    return ::QtProtobuf::QTime(from.hour(), from.minute(), from.second(), from.msec());
}

::QDate convert(const ::QtProtobuf::QDate &from) {
    return ::QDate(from.year(), from.month(), from.day());
}

::QtProtobuf::QDate convert(const ::QDate &from) {
    return ::QtProtobuf::QDate(from.year(), from.month(), from.day());
}

::QDateTime convert(const ::QtProtobuf::QDateTime &from) {
    return ::QDateTime(convert(from.date()), convert(from.time()));
}

::QtProtobuf::QDateTime convert(const ::QDateTime &from) {
    return ::QtProtobuf::QDateTime(convert(from.date()), convert(from.time()));
}

::QSize convert(const ::QtProtobuf::QSize &from) {
    return ::QSize(from.width(), from.height());
}

::QtProtobuf::QSize convert(const ::QSize &from) {
    return ::QtProtobuf::QSize(from.width(), from.height());
}

::QSizeF convert(const ::QtProtobuf::QSizeF &from) {
    return ::QSizeF(from.width(), from.height());
}

::QtProtobuf::QSizeF convert(const ::QSizeF &from) {
    return ::QtProtobuf::QSizeF(from.width(), from.height());
}

::QPoint convert(const ::QtProtobuf::QPoint &from) {
    return ::QPoint(from.x(), from.y());
}

::QtProtobuf::QPoint convert(const ::QPoint &from) {
    return ::QtProtobuf::QPoint(from.x(), from.y());
}

::QPointF convert(const ::QtProtobuf::QPointF &from) {
    return ::QPointF(from.x(), from.y());
}

::QtProtobuf::QPointF convert(const ::QPointF &from) {
    return ::QtProtobuf::QPointF(from.x(), from.y());
}

::QRect convert(const ::QtProtobuf::QRect &from) {
    return ::QRect(convert(from.topLeft()), convert(from.bottomRight()));
}

::QtProtobuf::QRect convert(const ::QRect &from) {
    return ::QtProtobuf::QRect(convert(from.topLeft()), convert(from.bottomRight()));
}

::QRectF convert(const ::QtProtobuf::QRectF &from) {
    return ::QRectF(convert(from.topLeft()), convert(from.bottomRight()));
}

::QtProtobuf::QRectF convert(const ::QRectF &from) {
    return ::QtProtobuf::QRectF(convert(from.topLeft()), convert(from.bottomRight()));
}

::QPolygon convert(const ::QtProtobuf::QPolygon &from) {
    ::QPolygon polygon;
    for (auto point : from.points()) {
        polygon.append(convert(*point));
    }
    return polygon;
}

::QtProtobuf::QPolygon convert(const ::QPolygon &from) {
    ::QtProtobuf::QPolygon polygon;
    for (auto point : from) {
        polygon.points().append(QSharedPointer<::QtProtobuf::QPoint>(new ::QtProtobuf::QPoint(convert(point))));
    }
    return polygon;
}

::QPolygonF convert(const ::QtProtobuf::QPolygonF &from) {
    ::QPolygonF polygon;
    for (auto point : from.points()) {
        polygon.append(convert(*point));
    }
    return polygon;
}

::QtProtobuf::QPolygonF convert(const ::QPolygonF &from) {
    ::QtProtobuf::QPolygonF polygon;
    for (auto point : from) {
        polygon.points().append(QSharedPointer<::QtProtobuf::QPointF>(new ::QtProtobuf::QPointF(convert(point))));
    }
    return polygon;
}

template <typename QType, typename PType>
void registerQtTypeHandler() {
    QtProtobufPrivate::registerHandler(qMetaTypeId<QType>(), {
                                           [](const QtProtobuf::QAbstractProtobufSerializer *serializer, const QVariant &value, const QtProtobuf::QProtobufMetaProperty &property, QByteArray &buffer) {
                                               PType object(convert(value.value<QType>()));
                                               buffer.append(serializer->serializeObject(&object, PType::protobufMetaObject, property));
                                           },
                                           [](const QtProtobuf::QAbstractProtobufSerializer *serializer, QtProtobuf::QProtobufSelfcheckIterator &it, QVariant &value) {
                                               PType object;
                                               serializer->deserializeObject(&object, PType::protobufMetaObject, it);
                                               value = QVariant::fromValue<QType>(convert(object));
                                           }, QtProtobufPrivate::ObjectHandler });
}

void qRegisterProtobufQtTypes() {
    registerQtTypeHandler<::QUrl, ::QtProtobuf::QUrl>();
    registerQtTypeHandler<::QChar, ::QtProtobuf::QChar>();
    registerQtTypeHandler<::QUuid, ::QtProtobuf::QUuid>();
    registerQtTypeHandler<::QTime, ::QtProtobuf::QTime>();
    registerQtTypeHandler<::QDate, ::QtProtobuf::QDate>();
    registerQtTypeHandler<::QDateTime, ::QtProtobuf::QDateTime>();
    registerQtTypeHandler<::QDateTime, ::QtProtobuf::QDateTime>();
    registerQtTypeHandler<::QSize, ::QtProtobuf::QSize>();
    registerQtTypeHandler<::QSizeF, ::QtProtobuf::QSizeF>();
    registerQtTypeHandler<::QPoint, ::QtProtobuf::QPoint>();
    registerQtTypeHandler<::QPointF, ::QtProtobuf::QPointF>();
    registerQtTypeHandler<::QRect, ::QtProtobuf::QRect>();
    registerQtTypeHandler<::QRectF, ::QtProtobuf::QRectF>();
    registerQtTypeHandler<::QPolygon, ::QtProtobuf::QPolygon>();
    registerQtTypeHandler<::QPolygonF, ::QtProtobuf::QPolygonF>();

    registerQtTypeHandler<::QColor, ::QtProtobuf::QColor>();
}

}
