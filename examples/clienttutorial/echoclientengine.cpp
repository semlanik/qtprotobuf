/*
 * MIT License
 *
 * Copyright (c) 2020 Alexey Edelev <semlanik@gmail.com>
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

#include "echoclientengine.h"

#include "tutorial_grpc.qpb.h"

#include <QGrpcHttp2Channel>
#include <QGrpcInsecureCredentials>

using namespace qtprotobuf::tutorial;
using namespace QtProtobuf;

EchoClientEngine::EchoClientEngine(QObject *parent) : QObject(parent), m_client(new EchoServiceClient), m_response(new EchoResponse)
{
    m_client->attachChannel(std::shared_ptr<QAbstractGrpcChannel>(new QGrpcHttp2Channel(QUrl("http://localhost:65000"),
                                                                                        QGrpcInsecureChannelCredentials()|QGrpcInsecureCallCredentials())));
}

void EchoClientEngine::request(qtprotobuf::tutorial::EchoRequest *req)
{
    m_client->Echo(*req, m_response.get());
}
