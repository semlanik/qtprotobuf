/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>
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

#include "qtprotobufquickplugin.h"
#include "qtprotobuftypes.h"
#include <private/qqmlmetatype_p.h>
#include <QDebug>

template<typename T>
static QVariant coverter(const QString &str) {
    QVariant value = QVariant::fromValue(str.toDouble());
    value.convert(qMetaTypeId<T>());
    return value;
}

void QtProtobufQuickPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("QtProtobuf"));
    qmlRegisterModule(uri, QTPROTOBUF_VERSION_MAJOR, QTPROTOBUF_VERSION_MINOR);
    QQmlMetaType::registerCustomStringConverter(qMetaTypeId<QtProtobuf::int32>(), coverter<QtProtobuf::int32>);
    QQmlMetaType::registerCustomStringConverter(qMetaTypeId<QtProtobuf::fixed32>(), coverter<QtProtobuf::fixed32>);
    QQmlMetaType::registerCustomStringConverter(qMetaTypeId<QtProtobuf::sfixed32>(), coverter<QtProtobuf::sfixed32>);
}
