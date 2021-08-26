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

#include "qtgrpcquickplugin.h"

#include "qquickgrpcstream_p.h"

#include <QDebug>
#include <QQmlEngine>
#include <QGrpcStatus>

using namespace QtProtobuf;

void QtGrpcQuickPlugin::registerTypes(const char *uri)
{
    // @uri QtGrpc
    Q_ASSERT(uri == QLatin1String("QtGrpc"));

    qmlRegisterUncreatableType<QtProtobuf::QGrpcStatus>("QtGrpc", QT_PROTOBUF_VERSION_MAJOR, QT_PROTOBUF_VERSION_MINOR, "GrpcStatus", "GrpcStatus only could be recevied from gRPC calls");
    qmlRegisterType<QQuickGrpcStream>("QtGrpc", QT_PROTOBUF_VERSION_MAJOR, QT_PROTOBUF_VERSION_MINOR, "GrpcStream");
}
