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

#include <QByteArray>
#include <stdexcept>

#include <QtProtobuf/qtprotobufglobal.h>

#pragma once //QProtobufSelfcheckIterator

QT_BEGIN_NAMESPACE

/*!
 * \ingroup QtProtobuf
 * \brief The QProtobufSelfcheckIterator class
 */
class Q_PROTOBUF_EXPORT QProtobufSelfcheckIterator
{
public:
    QProtobufSelfcheckIterator(const QByteArray &container) : m_sizeLeft(container.size())
      , m_containerSize(container.size())
      , m_it(container.begin()) {}

    QProtobufSelfcheckIterator(const QProtobufSelfcheckIterator &other) : m_sizeLeft(other.m_sizeLeft)
      , m_containerSize(other.m_containerSize)
      , m_it(other.m_it) {
        if (m_sizeLeft > m_containerSize || m_sizeLeft < 0) {
            QT_THROW(std::out_of_range("Container is less than required fields number. Deserialization failed"));
        }
    }

    explicit operator QByteArray::const_iterator&() { return m_it; }
    explicit operator QByteArray::const_iterator() const { return m_it; }

    char operator *() { return *m_it; }

    QProtobufSelfcheckIterator &operator ++() {
        --m_sizeLeft;
        if (m_sizeLeft < 0) {
            QT_THROW(std::out_of_range("Container is less than required fields number. Deserialization failed"));
        }
        ++m_it;
        return *this;
    }

    QProtobufSelfcheckIterator &operator --() {
        ++m_sizeLeft;
        if (m_sizeLeft > m_containerSize) {
            QT_THROW(std::out_of_range("Container is less than required fields number. Deserialization failed"));
        }
        --m_it;
        return *this;
    }

    QProtobufSelfcheckIterator &operator +=(int count) {
        m_sizeLeft -= count;
        if (m_sizeLeft < 0) {
            QT_THROW(std::out_of_range("Container is less than required fields number. Deserialization failed"));
        }
        m_it += count;
        return *this;
    }

    QProtobufSelfcheckIterator &operator -=(int count) {
        m_sizeLeft += count;
        if (m_sizeLeft > m_containerSize) {
            QT_THROW(std::out_of_range("Container is less than required fields number. Deserialization failed"));
        }
        m_it -= count;
        return *this;
    }

    QProtobufSelfcheckIterator &operator =(const QProtobufSelfcheckIterator &other) {
        if (this == &other) {
            return *this;
        }

        m_containerSize = other.m_containerSize;
        m_sizeLeft = other.m_sizeLeft;
        if (m_sizeLeft > m_containerSize || m_sizeLeft < 0) {
            QT_THROW(std::out_of_range("Container is less than required fields number. Deserialization failed"));
        }
        m_it = other.m_it;
        return *this;
    }

    bool operator ==(const QProtobufSelfcheckIterator &other) const {
        return other.m_it == m_it;
    }

    bool operator !=(const QProtobufSelfcheckIterator &other) const {
        return other.m_it != m_it;
    }

    bool operator ==(const QByteArray::const_iterator &other) const {
        return other == m_it;
    }

    bool operator !=(const QByteArray::const_iterator &other) const {
        return other != m_it;
    }

    const char *data() const {
        return m_it;
    }

    int size() const {
        return m_sizeLeft;
    }
private:
    int m_sizeLeft;
    int m_containerSize;
    QByteArray::const_iterator m_it;
};

inline QProtobufSelfcheckIterator operator +(const QProtobufSelfcheckIterator &it, int lenght) {
    QProtobufSelfcheckIterator itNew = it;
    return itNew += lenght;
}

QT_END_NAMESPACE
