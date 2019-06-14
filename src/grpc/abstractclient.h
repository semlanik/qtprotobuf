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

#pragma once

#include <memory>
#include <functional>
#include <type_traits>

#include <QObject>
#include <QByteArray>

#include <qtprotobuflogging.h>

#include "abstractchannel.h"
#include "asyncreply.h"

#include "qtgrpc_global.h"

namespace qtprotobuf {
/*!
*  \addtogroup QtGrpc
*  \{
*/

class AbstractChannel;

/*!
 * \brief The AbstractClient class
 */
class QTGRPCSHARED_EXPORT AbstractClient : public QObject
{
    Q_OBJECT
public:
    void attachChannel(const std::shared_ptr<AbstractChannel> &channel);

signals:
    void error(AbstractChannel::StatusCode code, const QString &errorText);

protected:
    AbstractClient(const QString &service, QObject *parent = nullptr);
    virtual ~AbstractClient();

    /*!
     * \brief A template of a sync RPC call
     *
     * \param method Name of the method to be called
     * \param arg Arguments (proto-message) passed to the method called
     * \param ret A pointer to memory with proto-message to write an RPC reply to
     */
    template<typename A, typename R>
    bool call(const QString &method, const A &arg, const QPointer<R> &ret) {
        if (ret.isNull()) {
            qProtoCritical() << "Unable to call method: " << method << ". Pointer to return data is null";
            return false;
        }

        QByteArray retData;
        if (AbstractChannel::StatusCode::Ok == call(method, arg.serialize(), retData)) {
            try {
                ret->deserialize(retData);
            } catch (std::invalid_argument &) {
                qProtoCritical() << "Response deserialization failed invalid field found";
                return false;
            } catch (std::out_of_range &) {
                qProtoCritical() << "Invalid size of received buffer";
                return false;
            } catch (...) {
                throw;
            }
            return true;
        }
        return false;
    }

    template<typename A>
    AsyncReply *call(const QString &method, const A &arg) {
        return call(method, arg.serialize());
    }

    template<typename A, typename R, typename C,
             typename std::enable_if_t<std::is_base_of<AbstractClient, C>::value, int> = 0>
    void subscribe(const QString &method, const A &arg, void(C::*signal)(const R &)) {
        subscribe_p(method, arg.serialize(), [this, signal](const QByteArray &data) {
            R ret;
            ret.deserialize(data);
            C *client = static_cast<C *>(this);
            (client->*signal)(ret);
        });
    }

    template<typename A, typename R>
    void subscribe(const QString &method, const A &arg, const QPointer<R> &ret) {
        if (ret.isNull()) {
            qProtoCritical() << "Unable to subscribe method: " << method << ". Pointer to return data is null";
            return;
        }

        subscribe_p(method, arg.serialize(), [ret](const QByteArray &data) {
            if (!ret.isNull()) {
                ret->deserialize(data);
            } else {
                qProtoCritical() << "Pointer to return data is null while subscription update received";
            }
        });
    }

private:
    AbstractChannel::StatusCode call(const QString &method, const QByteArray &arg, QByteArray &ret);
    AsyncReply *call(const QString &method, const QByteArray &arg);
    void subscribe_p(const QString &method, const QByteArray &arg, const std::function<void(const QByteArray &)> &handler);

    Q_DISABLE_COPY(AbstractClient)
    AbstractClient(AbstractClient &&) = delete;
    AbstractClient &operator =(AbstractClient &&) = delete;

    // PIMPL
    class AbstractClientPrivate *d;
};
/*! \} */
}
