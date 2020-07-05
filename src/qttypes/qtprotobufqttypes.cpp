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
#include <QMatrix4x4>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QTransform>
#include <QQuaternion>
#include <QImage>
#include <QBuffer>

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


::QMatrix4x4 convert(const ::QtProtobuf::QMatrix4x4 &from) {
    return ::QMatrix4x4(from.m11(), from.m12(), from.m13(), from.m14(),
                        from.m21(), from.m22(), from.m23(), from.m24(),
                        from.m31(), from.m32(), from.m33(), from.m34(),
                        from.m41(), from.m42(), from.m43(), from.m44());
}

::QtProtobuf::QMatrix4x4 convert(const ::QMatrix4x4 &from) {
    //QMatrix4x4::data returned in column-major format
    return ::QtProtobuf::QMatrix4x4(from.data()[0], from.data()[4], from.data()[8],  from.data()[12],
                                    from.data()[1], from.data()[5], from.data()[9],  from.data()[13],
                                    from.data()[2], from.data()[6], from.data()[10], from.data()[14],
                                    from.data()[3], from.data()[7], from.data()[11], from.data()[15]);
}

::QVector2D convert(const ::QtProtobuf::QVector2D &from) {
    return ::QVector2D(from.xpos(), from.ypos());
}

::QtProtobuf::QVector2D convert(const ::QVector2D &from) {
    return ::QtProtobuf::QVector2D(from.x(), from.y());
}

::QVector3D convert(const ::QtProtobuf::QVector3D &from) {
    return ::QVector3D(from.xpos(), from.ypos(), from.zpos());
}

::QtProtobuf::QVector3D convert(const ::QVector3D &from) {
    return ::QtProtobuf::QVector3D(from.x(), from.y(), from.z());
}

::QVector4D convert(const ::QtProtobuf::QVector4D &from) {
    return ::QVector4D(from.xpos(), from.ypos(), from.zpos(), from.wpos());
}

::QtProtobuf::QVector4D convert(const ::QVector4D &from) {
    return ::QtProtobuf::QVector4D(from.x(), from.y(), from.z(), from.w());
}

::QTransform convert(const ::QtProtobuf::QTransform &from) {
    return ::QTransform(from.m11(), from.m12(), from.m13(),
                        from.m21(), from.m22(), from.m23(),
                        from.m31(), from.m32(), from.m33());
}

::QtProtobuf::QTransform convert(const ::QTransform &from) {
    return ::QtProtobuf::QTransform(from.m11(), from.m12(), from.m13(),
                                    from.m21(), from.m22(), from.m23(),
                                    from.m31(), from.m32(), from.m33());
}

::QQuaternion convert(const ::QtProtobuf::QQuaternion &from) {
    return ::QQuaternion(from.scalar(), convert(from.vector()));
}

::QtProtobuf::QQuaternion convert(const ::QQuaternion &from) {
    return ::QtProtobuf::QQuaternion(from.scalar(), convert(from.vector()));
}

::QImage convert(const ::QtProtobuf::QImage &from) {
    return ::QImage::fromData(from.data(), from.format().toLatin1().data());
}

::QtProtobuf::QImage convert(const ::QImage &from) {
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    from.save(&buffer, "PNG");
    qProtoWarning() << "QImage always is sent in PNG format";
    return ::QtProtobuf::QImage(data, "PNG");
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
    registerQtTypeHandler<::QMatrix4x4, ::QtProtobuf::QMatrix4x4>();
    registerQtTypeHandler<::QVector2D, ::QtProtobuf::QVector2D>();
    registerQtTypeHandler<::QVector3D, ::QtProtobuf::QVector3D>();
    registerQtTypeHandler<::QVector4D, ::QtProtobuf::QVector4D>();
    registerQtTypeHandler<::QTransform, ::QtProtobuf::QTransform>();
    registerQtTypeHandler<::QQuaternion, ::QtProtobuf::QQuaternion>();
    registerQtTypeHandler<::QImage, ::QtProtobuf::QImage>();
}

}
