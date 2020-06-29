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

#pragma once

#include <QObject>
#include <QAbstractGrpcClient>
#include <QGrpcStatus>

class QJSValue;

namespace QtProtobuf {

class QGrpcSubscription;

class QQuickGrpcSubscription : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractGrpcClient *client READ client WRITE setClient NOTIFY clientChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString method READ method WRITE setMethod NOTIFY methodChanged)
    Q_PROPERTY(QObject *argument READ argument WRITE setArgument NOTIFY argumentChanged)

public:
    QQuickGrpcSubscription(QObject *parent = nullptr);
    ~QQuickGrpcSubscription() = default;

    QAbstractGrpcClient *client() const {
        return m_client;
    }

    bool enabled() const {
        return m_enabled;
    }

    QString method() const {
        return m_method;
    }

    QObject *argument() const {
        return m_argument;
    }

    void setClient(QAbstractGrpcClient *client) {
        if (m_client == client) {
            return;
        }

        m_client = client;
        emit clientChanged();
        updateSubscription();
    }

    void setEnabled(bool enabled) {
        if (m_enabled == enabled) {
            return;
        }

        m_enabled = enabled;
        emit enabledChanged();
        updateSubscription();
    }

    void setMethod(QString method) {
        if (m_method == method) {
            return;
        }

        m_method = method;
        emit methodChanged();
        updateSubscription();
    }

    void setArgument(QObject *argument) {
        if (m_argument == argument) {
            return;
        }

        m_argument = argument;
        emit argumentChanged();
        updateSubscription();
    }

signals:
    void updated(const QJSValue &value);
    void error(const QtProtobuf::QGrpcStatus &status);

    void clientChanged();
    void methodChanged();
    void enabledChanged();
    void argumentChanged();

private:
    void updateSubscription();
    bool subscribe();
    QPointer<QAbstractGrpcClient> m_client;
    bool m_enabled;
    QString m_method;
    QPointer<QObject> m_argument;
    QGrpcSubscription *m_subscription;
    QObject *m_returnValue;
};

}
