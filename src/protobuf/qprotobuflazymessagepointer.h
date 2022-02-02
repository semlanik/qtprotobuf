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

#pragma once //QProtobufLazyMessagePointer

#include <QtProtobuf/qtprotobufglobal.h>
#include <QtCore/qobject.h>

#if defined(QT_QML_LIB)
#  include <QQmlEngine>
#endif

#include <memory>
#include <type_traits>

QT_BEGIN_NAMESPACE
template <typename T>
class QProtobufLazyMessagePointer final {
public:
    QProtobufLazyMessagePointer(T *p = nullptr) : m_ptr(p) {}

    virtual ~QProtobufLazyMessagePointer() {
        checkAndRelease();
        QObject::disconnect(m_destroyed);
    }

    typename std::add_lvalue_reference<T>::type operator *() const {
        if (m_ptr == nullptr) {
            m_ptr.reset(new T);
        }
        return *m_ptr;
    }

    T *operator->() const noexcept {
        if (m_ptr == nullptr) {
            m_ptr.reset(new T);
        }
        m_ptr.operator->();
    }

    T *get() const {
        if (m_ptr == nullptr) {
            m_ptr.reset(new T);
        }
        return m_ptr.get();
    }

    bool operator ==(const QProtobufLazyMessagePointer &other) const {
        return (m_ptr == nullptr && other.m_ptr == nullptr)
                || (other.m_ptr == nullptr && *m_ptr == T{})
                || (m_ptr == nullptr && *other.m_ptr == T{})
                || (m_ptr != nullptr && other.m_ptr != nullptr && *m_ptr == *other.m_ptr);
    }

    bool operator !=(const QProtobufLazyMessagePointer &other) const {
        return !operator ==(other);
    }

    void reset(T *p) const {
        checkAndRelease();
        QObject::disconnect(m_destroyed);
        m_destroyed = QObject::connect(p, &QObject::destroyed, [this] {
            QObject::disconnect(m_destroyed);
            m_ptr.release();
        });

        m_ptr.reset(p);
    }

    QProtobufLazyMessagePointer(QProtobufLazyMessagePointer &&other) : m_ptr(std::move(other.m_ptr)) {}
    QProtobufLazyMessagePointer &operator =(QProtobufLazyMessagePointer &&other) {
        m_ptr = std::move(other.m_ptr);
    }

    explicit operator bool() const noexcept {
        return m_ptr.operator bool();
    }

private:
    void checkAndRelease() const {
#if defined(QT_QML_LIB)
        bool qmlCheck = QQmlEngine::objectOwnership(m_ptr.get()) == QQmlEngine::JavaScriptOwnership;
#else
        bool qmlCheck = true;
#endif
        if (m_ptr != nullptr && qmlCheck) {
            //If the object owned by QML it's QML's responsibility to clean it up
            m_ptr.release();
            QObject::disconnect(m_destroyed);
        }
    }

    QProtobufLazyMessagePointer(const QProtobufLazyMessagePointer&) = delete;
    QProtobufLazyMessagePointer &operator =(const QProtobufLazyMessagePointer&) = delete;
    mutable std::unique_ptr<T> m_ptr;
    mutable QMetaObject::Connection m_destroyed;
};

QT_END_NAMESPACE
