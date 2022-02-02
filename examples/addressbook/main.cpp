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

#include "addressbookengine.h"
#include "addressbook.qpb.h"

#include <QMetaProperty>
#include <QQmlPropertyMap>

using namespace qtprotobuf::examples;

int main(int argc, char *argv[])
{
    qRegisterProtobufTypes();

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    qmlRegisterType<ContactsListModel>("examples.addressbook", 1, 0, "ContactsListModel");
    QGuiApplication app(argc, argv);

    qmlRegisterSingletonType<AddressBookEngine>("examples.addressbook", 1, 0, "AddressBookEngine", [](QQmlEngine *engine, QJSEngine *) -> QObject *{
        static AddressBookEngine abEngine;
        engine->setObjectOwnership(&abEngine, QQmlEngine::CppOwnership);
        return &abEngine;
    });

    QQmlApplicationEngine engine;

    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
