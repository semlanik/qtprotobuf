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


#include "qquickgrpcsubscription_p.h"

#include <QGrpcSubscription>
#include <QJSEngine>
#include <QQmlEngine>

using namespace QtProtobuf;

QQuickGrpcSubscription::QQuickGrpcSubscription(QObject *parent) : QObject(parent)
  , m_enabled(false)
  , m_returnValue(nullptr)
{
}

QQuickGrpcSubscription::~QQuickGrpcSubscription()
{
    if (!m_subscription.isNull()) {
        m_subscription->cancel();
    }
    delete m_returnValue;
}


void QQuickGrpcSubscription::updateSubscription()
{
    if (!m_subscription.isNull()) {
        m_subscription->cancel();
        m_subscription = nullptr;
    }

    if (m_returnValue != nullptr) {
        m_returnValue->deleteLater(); //TODO: probably need to take care about return value cleanup other way. It's just reminder about weak memory management.
        m_returnValue = nullptr;
    }

    if (m_client.isNull() || m_method.isEmpty() || !m_enabled || m_argument.isNull()) {
        return;
    }

    if (!subscribe()) {
        setEnabled(false);
    }
}

bool QQuickGrpcSubscription::subscribe()
{
    QString uppercaseMethodName = m_method;
    uppercaseMethodName.replace(0, 1, m_method[0].toUpper());
    const QMetaObject *metaObject = m_client->metaObject();
    QMetaMethod method;
    for (int i = 0; i < metaObject->methodCount(); i++) {
        if (QString("qmlSubscribe%1Updates_p").arg(uppercaseMethodName) == metaObject->method(i).name()) {
            method = metaObject->method(i);
            break;
        }
    }

    QString errorString;
    if (!method.isValid()) {
        errorString = m_method + "is not either server or bidirectional stream.";
        qProtoWarning() << errorString;
        error({QGrpcStatus::Unimplemented, errorString});
        return false;
    }

    if (method.parameterCount() < 2) {
        errorString = QString("Unable to call ") + method.name() + ". Invalid arguments set.";
        qProtoWarning() << errorString;
        error({QGrpcStatus::InvalidArgument, errorString});
        return false;
    }

    QMetaType argumentPointerMetaType(method.parameterType(0));
    if (argumentPointerMetaType.metaObject() != m_argument->metaObject()) {
        errorString = QString("Unable to call ") + method.name() + ". Argument type mismatch: '" + method.parameterTypes().at(0) + "' expected, '" + m_argument->metaObject()->className() + "' provided";
        qProtoWarning() << errorString;
        error({QGrpcStatus::InvalidArgument, errorString});
        return false;
    }

    QMetaType argumentMetaType(QMetaType::type(m_argument->metaObject()->className()));
    if (!argumentMetaType.isValid()) {
        errorString = QString("Argument of type '") + m_argument->metaObject()->className() + "' is not registred in metatype system";
        qProtoWarning() << errorString;
        error({QGrpcStatus::InvalidArgument, errorString});
        return false;
    }

    QObject *argument = reinterpret_cast<QObject*>(argumentMetaType.create(m_argument));
    if (argument == nullptr) {
        errorString = "Unable to create argument copy. Unknown metatype system error";
        qProtoWarning() << errorString;
        error({QGrpcStatus::InvalidArgument, errorString});
        return false;
    }
    argument->deleteLater(); //TODO: probably need to take care about temporary argument value cleanup other way. It's just reminder about weak memory management.

    QMetaType returnPointerType(method.parameterType(1));
    if (!returnPointerType.isValid()) {
        errorString = QString("Return type argument of type '") + method.parameterTypes().at(1) + "' is not registred in metatype system";
        qProtoWarning() << errorString;
        error({QGrpcStatus::InvalidArgument, errorString});
        return false;
    }

    QMetaType returnMetaType(QMetaType::type(returnPointerType.metaObject()->className()));
    if (!returnMetaType.isValid()) {
        errorString = QString("Unable to allocate return value. '") + returnPointerType.metaObject()->className() + "' is not registred in metatype system";
        qProtoWarning() << errorString;
        error({QGrpcStatus::InvalidArgument, errorString});
        return false;
    }

    m_returnValue = reinterpret_cast<QObject*>(returnMetaType.create());
    qmlEngine(this)->setObjectOwnership(m_returnValue, QQmlEngine::CppOwnership);

    if (m_returnValue == nullptr) {
        errorString = "Unable to allocate return value. Unknown metatype system error";
        qProtoWarning() << errorString;
        error({QGrpcStatus::Unknown, errorString});
        return false;
    }

    QGrpcSubscription *subscription = nullptr;
    bool ok = method.invoke(m_client, Qt::DirectConnection,
                                      QGenericReturnArgument("QtProtobuf::QGrpcSubscription*", static_cast<void *>(&subscription)),
                                      QGenericArgument(method.parameterTypes().at(0).data(), static_cast<const void *>(&argument)),
                                      QGenericArgument(method.parameterTypes().at(1).data(), static_cast<const void *>(&m_returnValue)));
    if (!ok || subscription == nullptr) {
        errorString = QString("Unable to call ") + m_method + "invalidate subscription.";
        qProtoWarning() << errorString;
        error({QGrpcStatus::Unknown, errorString});
        return false;
    }

    m_subscription = subscription;

    connect(m_subscription, &QGrpcSubscription::updated, this, [this](){
        updated(qjsEngine(this)->toScriptValue(m_returnValue));
    });

    connect(m_subscription, &QGrpcSubscription::error, this, &QQuickGrpcSubscription::error);//TODO: Probably it's good idea to disable subscription here

    connect(m_subscription, &QGrpcSubscription::finished, this, [this](){ setEnabled(false); });

    return true;
}
