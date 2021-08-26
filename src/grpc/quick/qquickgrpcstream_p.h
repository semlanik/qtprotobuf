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
 * \class GrpcStream
 * \brief GrpcStream provides access to gRPC streams from QML.
 *
 * \details GrpcStream might be used from QML code to receive updates for gRPC server or bidirectional streaming methods.
 * Follwing properties should be provided and can not be empty, to subscribe streaming method:
 * - client
 * - method
 * - argument
 * Changing any of this properties cause stream cancelation and re-initialization.
 *
 * \subsection Properties
 * \subsubsection client QtProtobuf::QAbstractGrpcClient *client
 * \details Client used for stream.
 *
 * \subsubsection enabled bool enabled
 * \details Controls stream status. If stream is active, switch this flag to 'false'
 * to cancel stream. Switching to 'false' keeps all fields ready to restore stream.
 *
 * \subsubsection method QString method
 * \details The name of streaming method that will be used for stream.
 *
 * \subsubsection argument QObject *argument
 * \details Pointer to argument that will be used for stream.
 *
 * \subsubsection returnValue QObject *returnValue
 * \details Value returned by the stream (Note that it is the same "return" object passed by the "updated" signal)
 *
 * \subsection Signals
 * \subsubsection updated messageReceived(ReturnType value)
 * \details The signal notifies about received update for stream. It provides "return" value ready for use in QML.
 * \code
 * GrpcStream {
 *     ...
 *     onMessageReceived: {
 *         //Use value passed as argument to binding
 *         ...
 *     }
 * }
 * \endcode
 *
 * \subsubsection error error(QtProtobuf::QGrpcStatus status)
 * \details The signal notifies about error occured for stream. Provides GrpcStatus as argument to assigned handler.
 * \note Some errors at stream initialization phase disable GrpcStream
 *
 * \code
 * GrpcStream {
 *     id: myStream
 *     ...
 *     onError: {
 *         console.log("Stream error: " + status.code + " " + status.message)
 *     }
 * }
 * \endcode
 */

class QJSValue;

namespace QtProtobuf {

class QGrpcStream;

//! \private
class QQuickGrpcStream : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractGrpcClient *client READ client WRITE setClient NOTIFY clientChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString method READ method WRITE setMethod NOTIFY methodChanged)
    Q_PROPERTY(QObject *argument READ argument WRITE setArgument NOTIFY argumentChanged)
    Q_PROPERTY(QObject *returnValue READ returnValue NOTIFY returnValueChanged)

public:
    QQuickGrpcStream(QObject *parent = nullptr);
    ~QQuickGrpcStream();

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
        updateStream();
    }

    void setEnabled(bool enabled) {
        if (m_enabled == enabled) {
            return;
        }

        m_enabled = enabled;
        emit enabledChanged();
        updateStream();
    }

    void setMethod(QString method) {
        if (m_method == method) {
            return;
        }

        m_method = method;
        emit methodChanged();
        updateStream();
    }

    void setArgument(QObject *argument) {
        if (m_argument == argument) {
            return;
        }

        m_argument = argument;
        emit argumentChanged();
        updateStream();
    }

signals:
    void messageReceived(const QJSValue &value);
    void error(const QtProtobuf::QGrpcStatus &status);

    void clientChanged();
    void methodChanged();
    void enabledChanged();
    void argumentChanged();
    void returnValueChanged();

private:
    void updateStream();
    bool subscribe();
    QPointer<QAbstractGrpcClient> m_client;
    bool m_enabled;
    QString m_method;
    QPointer<QObject> m_argument;
    std::shared_ptr<QGrpcStream> m_stream;
    QObject *m_returnValue;
};

}
