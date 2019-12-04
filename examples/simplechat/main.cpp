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

#include <QGuiApplication>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "simplechat.qpb.h"
#include "qtprotobuf_global.qpb.h"

#include "simplechatengine.h"

#include <QMetaProperty>
#include <QQmlPropertyMap>
#include <QCryptographicHash>

using namespace qtprotobuf::examples;

int main(int argc, char *argv[])
{
    QtProtobuf::qRegisterProtobufTypes();
    qtprotobuf::examples::qRegisterProtobufTypes();

    QGuiApplication app(argc, argv);

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    qmlRegisterUncreatableType<ChatMessageModel>("examples.simplechat",  1, 0, "ChatMessageModel", "");

    QQmlApplicationEngine engine;

    SimpleChatEngine scEngine;
    engine.rootContext()->setContextProperty("scEngine", &scEngine);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
