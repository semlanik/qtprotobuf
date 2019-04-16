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

#include <QString>
#include <QByteArray>

namespace qtprotobuf {

class AsyncReply;

class AbstractChannel
{
public:
    /*!
     * \brief The Channel StatusCodes
     */
    enum StatusCodes {
        Ok = 0,                 //!< No error
        Cancelled = 1,          //!< The operation was cancelled, typically by the caller
        Unknown = 2,            //!< Unknown error
        InvalidArgument = 3,    //!< The client specified an invalid argument
        DeadlineExceeded = 4,   //!< The deadline expired before the operation could complete
        NotFound = 5,           //!< Some requested entity (e.g., file or directory) was not found
        AlreadyExists = 6,      //!< The entity that a client attempted to create (e.g., file or directory) already exists. 	409 Conflict
        PermissionDenied = 7,   //!< The caller does not have permission to execute the specified operation. PERMISSION_DENIED must not be used for rejections caused by exhausting some resource (use RESOURCE_EXHAUSTED instead for those errors). PERMISSION_DENIED must not be used if the caller can not be identified (use UNAUTHENTICATED instead for those errors). This error code does not imply the request is valid or the requested entity exists or satisfies other pre-conditions. 	403 Forbidden
        Unauthenticated = 16,   //!< The request does not have valid authentication credentials for the operation
        ResourceExhausted = 8,  //!< Some resource has been exhausted, perhaps a per-user quota, or perhaps the entire file system is out of space
        FailedPrecondition = 9, //!< The operation was rejected because the system is not in a state required for the operation's execution
        Aborted = 10,           //!< The operation was aborted, typically due to a concurrency issue such as a sequencer check failure or transaction abort
        OutOfRange = 11,        //!< The operation was attempted past the valid range
        Unimplemented = 12,     //!< The operation is not implemented or is not supported/enabled in this service
        Internal = 13,          //!< Internal errors. This means that some invariants expected by the underlying system have been broken.
        Unavailable = 14,       //!< The service is currently unavailable
        DataLoss = 15,          //!< Unrecoverable data loss or corruption
    };

    virtual StatusCodes call(const QString &method, const QString &service, const QByteArray &args, QByteArray &ret) = 0;
    /*!
     * \brief call
     * \param method
     * \param service
     * \param args
     * \param ret AsyncReply that will be returned to end-point user to read data once ready. AsyncReply owned by AbstractClient only.
     * \return
     */
    virtual void call(const QString &method, const QString &service, const QByteArray &args, qtprotobuf::AsyncReply *ret) = 0;

protected:
    AbstractChannel() = default;
    virtual ~AbstractChannel() = default;

    virtual void abort(AsyncReply *) {
        assert("Abort is not supported by used channel");
    }

    friend class AsyncReply;
private:
    Q_DISABLE_COPY(AbstractChannel)
};

}
