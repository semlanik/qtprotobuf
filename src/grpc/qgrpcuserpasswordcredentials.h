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

#pragma once //QGrpcUserPasswordCredentials

#include "qgrpccredentials.h"

namespace QtProtobuf {

constexpr char defaultUserFieldName[] = "user-name";
constexpr char defaultPasswordFieldName[] = "user-password";

/*!
 * \ingroup QtGrpc
 * \brief The QGrpcUserPasswordCredentials class is reference implementation of simple user-password call authentication.
 * \details You may manually specify UserFieldName and PasswordFieldName in class template parameters to change header keys.
 *          Default values for:
 *          \code{cpp}
 *          constexpr char defaultUserFieldName[] = "user-name";
 *          constexpr char defaultPasswordFieldName[] = "user-password";
 *          \endcode
 */
template<const char *UserFieldName = defaultUserFieldName,
         const char *PasswordFieldName = defaultPasswordFieldName>
class QGrpcUserPasswordCredentials final : public QGrpcCallCredentials
{
public:
    /*!
     * \brief QGrpcUserPasswordCredentials constucts QGrpcUserPasswordCredentials
     * \param userName value that will be assigned to UserFieldName header for each gRPC call
     * \param password value that will be assigned to PasswordFieldName header for each gRPC call
     */
    QGrpcUserPasswordCredentials(const QString &userName, const QString &password) :
        m_map(QtProtobuf::QGrpcCredentialMap{{QLatin1String(UserFieldName), QVariant::fromValue(userName)},
                                      {QLatin1String(PasswordFieldName), QVariant::fromValue(password)}})
	{}

    //!\private
	QtProtobuf::QGrpcCredentialMap operator()() {
		return m_map;
	}
private:
	QGrpcUserPasswordCredentials() = default;
    QtProtobuf::QGrpcCredentialMap m_map;
};
}
