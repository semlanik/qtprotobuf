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

/*!
 * \ingroup QtGrpcQML
 * \class GrpcSubscription
 * \brief GrpcSubscription provides access to gRPC subscriptions from QML.
 *
 * \details GrpcSubscription might be used from QML code to receive updates for gRPC server or bidirectional streaming methods.
 * Follwing properties should be provided and can not be empty, to subscribe streaming method:
 * - client
 * - method
 * - argument
 * Changing any of this properties cause subscription cancelation and re-initialization.
 *
 * \subsection Properties
 * \subsubsection client QtProtobuf::QAbstractGrpcClient *client
 * \details Client used for subscription.
 *
 * \subsubsection enabled bool enabled
 * \details Controls subscription status. If subscription is active, switch this flag to 'false'
 * to cancel subscription. Switching to 'false' keeps all fields ready to restore subscription.
 *
 * \subsubsection method QString method
 * \details The name of streaming method that will be used for subscription.
 *
 * \subsubsection argument QObject *argument
 * \details Pointer to argument that will be used for subscription.
 *
 * \subsubsection returnValue QObject *returnValue
 * \details Value returned by the subscription (Note that it is the same "return" object passed by the "updated" signal)
 *
 * \subsection Signals
 * \subsubsection updated updated(ReturnType value)
 * \details The signal notifies about received update for subscription. It provides "return" value ready for use in QML.
 * \code
 * GrpcSubscription {
 *     ...
 *     onUpdated: {
 *         //Use value passed as argument to binding
 *         ...
 *     }
 * }
 * \endcode
 *
 * \subsubsection error error(QtProtobuf::QGrpcStatus status)
 * \details The signal notifies about error occured for subscription. Provides GrpcStatus as argument to assigned handler.
 * \note Some errors at subscription initialization phase disable GrpcSubscription
 *
 * \code
 * GrpcSubscription {
 *     id: mySubscription
 *     ...
 *     onError: {
 *         console.log("Subscription error: " + status.code + " " + status.message)
 *     }
 * }
 * \endcode
 */

class QJSValue;

namespace QtProtobuf {

class QGrpcSubscription;

//! \private
class QQuickGrpcSubscription : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractGrpcClient *client READ client WRITE setClient NOTIFY clientChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString method READ method WRITE setMethod NOTIFY methodChanged)
    Q_PROPERTY(QObject *argument READ argument WRITE setArgument NOTIFY argumentChanged)
    Q_PROPERTY(QObject *returnValue READ returnValue NOTIFY returnValueChanged)

public:
    QQuickGrpcSubscription(QObject *parent = nullptr);
    ~QQuickGrpcSubscription();

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

    QObject *returnValue() const {
        return m_returnValue;
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
    void returnValueChanged();

private:
    void updateSubscription();
    bool subscribe();
    QPointer<QAbstractGrpcClient> m_client;
    bool m_enabled;
    QString m_method;
    QPointer<QObject> m_argument;
    std::shared_ptr<QGrpcSubscription> m_subscription;
    QObject *m_returnValue;
};

}
