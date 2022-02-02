/*
 * MIT License
 *
 * Copyright (c) 2021 Alexey Edelev <semlanik@gmail.com>
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

#include <QtQuickTest>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlExtensionPlugin>

#include "extra_namespace.qpb.h"

using namespace MyTestNamespace::qtprotobufnamespace::tests;

class TestSetup : public QObject {
    Q_OBJECT
public:
    TestSetup() {
        qRegisterProtobufTypes();
        Q_PROTOBUF_IMPORT_QUICK_PLUGIN()
    }
    ~TestSetup() = default;
public slots:
    void qmlEngineAvailable(QQmlEngine *engine)
    {
        QVersionNumber qtVersion = QVersionNumber::fromString(qVersion());
        engine->rootContext()->setContextProperty("qVersion", qVersion());
        engine->rootContext()->setContextProperty("qVersionMajor", qtVersion.majorVersion());
        engine->rootContext()->setContextProperty("qVersionMinor", qtVersion.minorVersion());
    }
};
