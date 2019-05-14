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

#include <QByteArray>
#include <stdexcept>

#include "qtprotobuf_global.h"

#pragma once

namespace qtprotobuf {

class QTPROTOBUFSHARED_EXPORT SelfcheckIterator
{
public:
    SelfcheckIterator(const QByteArray& container) : m_sizeLeft(container.size())
      , m_containerSize(container.size())
      , m_it(container.begin()){}

    SelfcheckIterator(const SelfcheckIterator& other) : m_sizeLeft(other.m_sizeLeft)
      ,m_containerSize(other.m_containerSize)
      , m_it(other.m_it){}

    explicit operator QByteArray::const_iterator&() { return m_it; }
    explicit operator QByteArray::const_iterator() const { return m_it; }

    char operator *() { return *m_it; }

    SelfcheckIterator& operator ++() {
        --m_sizeLeft;
        if (m_sizeLeft < 0) {
            throw std::out_of_range("Container is less than required fields number. Deserialization failed");
        }
        ++m_it;
        return *this;
    }

    SelfcheckIterator& operator --() {
        ++m_sizeLeft;
        if (m_sizeLeft >= m_containerSize) {
            throw std::out_of_range("Container is less than required fields number. Deserialization failed");
        }
        --m_it;
        return *this;
    }

    SelfcheckIterator& operator +=(int count) {
        m_sizeLeft -= count;
        if (m_sizeLeft < 0) {
            throw std::out_of_range("Container is less than required fields number. Deserialization failed");
        }
        m_it += count;
        return *this;
    }

    SelfcheckIterator& operator -=(int count) {
        m_sizeLeft += count;
        if (m_sizeLeft >= m_containerSize) {
            throw std::out_of_range("Container is less than required fields number. Deserialization failed");
        }
        m_it -= count;
        return *this;
    }

    SelfcheckIterator& operator =(SelfcheckIterator& other) {
        if (this == &other) {
            return *this;
        }

        m_containerSize = other.m_containerSize;
        m_sizeLeft = other.m_sizeLeft;
        if (m_sizeLeft >= m_containerSize) {
            throw std::out_of_range("Container is less than required fields number. Deserialization failed");
        }
        m_it = other.m_it;
        return *this;
    }

    bool operator ==(const SelfcheckIterator& other) const {
        return other.m_it == m_it;
    }

    bool operator !=(const SelfcheckIterator& other) const {
        return other.m_it != m_it;
    }

    bool operator ==(const QByteArray::const_iterator& other) const {
        return other == m_it;
    }

    bool operator !=(const QByteArray::const_iterator& other) const {
        return other != m_it;
    }
private:
    int m_sizeLeft;
    int m_containerSize;
    QByteArray::const_iterator m_it;
};

inline SelfcheckIterator operator +(const SelfcheckIterator& it, int lenght) {
    SelfcheckIterator itNew = it;
    return itNew += lenght;
}

}
