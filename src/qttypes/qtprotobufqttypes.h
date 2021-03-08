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

#pragma once //QtProtobufQtTypes

#include <qtprotobufqttypesglobal.h>

/*!
 * \defgroup QtProtobufQtTypes
 * \brief Provides support of native Qt types
 *
 * \details Like Google Protocol Buffers provide bench of standard types, QtProtobuf give you possibility to use Qt types directly in your projects.
 * QtProtobufQtTypes library makes this possible.
 *
 * List of supported types:
 *
 * \par \b QtCore
 * - QUrl
 * - QChar
 * - QUuid
 * - QTime
 * - QDate
 * - QDateTime
 * - QSize
 * - QSizeF
 * - QPoint
 * - QPointF
 * - QRect
 * - QRectF
 * - QPolygon
 * - QPolygonF
 *
 * \par \b QtGui
 * - QColor
 * - QMatrix4x4
 * - QVector2D
 * - QVector3D
 * - QVector4D
 * - QTransform
 * - QQuaternion
 * - QImage
 *
 * \note To request support of any other <a href="https://doc.qt.io/qt-5/qmetatype.html#Type-enum">Qt meta type</a> please submit feature issue in project <a href="https://github.com/semlanik/qtprotobuf/issues">bugtracker</a>.
 *
 * \subsubsection Usage
 *
 * To enable Qt types support add QtProtobufQtTypes as dependency to CMake project:
 * \code
 * ...
 * find_package(QtProtobufProject CONFIG COMPONENTS QtProtobuf QtProtobufQtTypes REQUIRED)
 * ... #After target creation
 * target_link_libraries(${TARGET} PRIVATE ${QT_PROTOBUF_PROJECT}::QtProtobufQtTypes)
 * \endcode
 *
 * Starting from this point you are almost complete preparation. Unlike automatical registration of generated code, QtProtobufQtTypes requires additional intialization step.
 * Before any serialization/deserialization of messages that use Qt types as fields, call registration method:
 * \code
 * ... //E.g. somewhere in main.cpp
 * QtProtobuf::qRegisterProtobufQtTypes();
 * ...
 * \endcode
 *
 * All supported message are described in special .proto files:
 *
 * - QtCore.proto - contains description of Qt types from QtCore module
 * - QtGui.proto - contains description of Qt types from QtGui module
 *
 * These files also useful if you would like to generate code for other languages or frameworks.
 * They located in project include directories, but no need to specify anything manualy, qtprotobuf_generate macro takes care about all side work for you.
 *
 * Import required Qt types module in your interface .proto file, e.g.:
 * \code
 * syntax = "proto3";
 *
 * package project.module.component;
 *
 * import "QtProtobuf/QtCore.proto";
 *
 * message QUrlMessage {
 *     QtProtobuf.QUrl url = 1;
 * }
 * \endcode
 *
 * QtProtobuf generator detects fields of type is located in QtProtobuf package and use Qt type directly instead of complex message generation. This give you
 * flexibility to use Qt types without additional convertion steps.
 *
 * In generated code you will see following property ready to use:
 *
 * \code
 * class QUrlMessage : public QObject
 * {
 * ...
 * Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged SCRIPTABLE true)
 * ...
 * }
 * \endcode
 */

namespace QtProtobuf {
/*!
 * \brief qRegisterProtobufQtTypes registers serializers set for Qt types supported by QtProtobufQtTypes
 * \note Call it before any serialization\deserialization of messages that use QtProtobufQtTypes directly on indirectly
 */
Q_PROTOBUF_QT_TYPES_EXPORT void qRegisterProtobufQtTypes();
}
