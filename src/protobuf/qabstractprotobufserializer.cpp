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

#include <QMetaProperty>
#include <QVariant>
#include <QMetaObject>
#include <QMutex>

#include "qabstractprotobufserializer.h"

using namespace QtProtobuf;

namespace  {

/*!
 * \private
 * \brief The HandlersRegistry is container to store mapping between metatype identifier and serialization handlers.
 */
struct HandlersRegistry {

    void registerHandler(int userType, const QtProtobufPrivate::SerializationHandler &handlers) {
        QMutexLocker locker(&m_lock);
        m_registry[userType] = handlers;
    }

    QtProtobufPrivate::SerializationHandler &findHandler(int userType) {
        QMutexLocker locker(&m_lock);
        auto it = m_registry.find(userType);
        if (it != m_registry.end()) {
            return it->second;
        }
        return empty;
    }

    static HandlersRegistry &instance() {
        static HandlersRegistry _instance;
        return _instance;
    }
private:
    QMutex m_lock;
    std::unordered_map<int/*metatypeid*/, QtProtobufPrivate::SerializationHandler> m_registry;
    static QtProtobufPrivate::SerializationHandler empty;
};

QtProtobufPrivate::SerializationHandler HandlersRegistry::empty{};
}

void QtProtobufPrivate::registerHandler(int userType, const QtProtobufPrivate::SerializationHandler &handlers)
{
    HandlersRegistry::instance().registerHandler(userType, handlers);
}

QtProtobufPrivate::SerializationHandler &QtProtobufPrivate::findHandler(int userType)
{
    return HandlersRegistry::instance().findHandler(userType);
}
