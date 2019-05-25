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
#include "qtprotobufvaluetypes_p.h"
#include <private/qqmlmetatype_p.h>
#include <QDebug>

void QtProtobufQuickPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("QtProtobuf"));
    QQmlMetaType::registerCustomStringConverter(qMetaTypeId<qtprotobuf::int32>(), [](const QString &s)
    {
        return QVariant::fromValue(qtprotobuf::int32(s.toInt()));
    });
    QQmlMetaType::registerCustomStringConverter(qMetaTypeId<qtprotobuf::int64>(), [](const QString &s)
    {
        return QVariant::fromValue(qtprotobuf::int64(s.toLongLong()));
    });
    QQmlMetaType::registerCustomStringConverter(qMetaTypeId<qtprotobuf::fixed32>(), [](const QString &s)
    {
        return QVariant::fromValue(qtprotobuf::fixed32(s.toUInt()));
    });
    QQmlMetaType::registerCustomStringConverter(qMetaTypeId<qtprotobuf::fixed64>(), [](const QString &s)
    {
        return QVariant::fromValue(qtprotobuf::fixed64(s.toULongLong()));
    });
    QQmlMetaType::registerCustomStringConverter(qMetaTypeId<qtprotobuf::sfixed32>(), [](const QString &s)
    {
        return QVariant::fromValue(qtprotobuf::sfixed32(s.toInt()));
    });
    QQmlMetaType::registerCustomStringConverter(qMetaTypeId<qtprotobuf::sfixed64>(), [](const QString &s)
    {
        return QVariant::fromValue(qtprotobuf::sfixed64(s.toLongLong()));
    });
    qtprotobuf::registerValueTypes();
}

QtProtobufQuickPlugin::~QtProtobufQuickPlugin()
{
    qtprotobuf::releaseValueTypes();
}
