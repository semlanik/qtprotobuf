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

#include <QtQml/private/qqmlglobal_p.h>
#include <QtQml/private/qv4engine_p.h>
#include <QtQml/private/qv4object_p.h>
#include <QtQml/private/qv8engine_p.h>

#include "qtprotobufvaluetypes_p.h"
#include "qtprotobuftypes.h"

namespace qtprotobuf {

class QtProtobufValueTypeProvider : public QQmlValueTypeProvider
{
    QtProtobufValueTypeProvider() = default;
    Q_DISABLE_COPY(QtProtobufValueTypeProvider)
    QtProtobufValueTypeProvider(QtProtobufValueTypeProvider &&) = delete;
    QtProtobufValueTypeProvider &operator =(QtProtobufValueTypeProvider &&) = delete;
public:
    static QtProtobufValueTypeProvider *instance() {
        static QtProtobufValueTypeProvider provider;
        return &provider;
    }

    const QMetaObject *getMetaObjectForMetaType(int type) override {
        qDebug() << "getMetaObjectForMetaType: " <<  type << qMetaTypeId<fixed32>();
        if (type == qMetaTypeId<fixed32>()) {
            return &QtProtobufFixed32ValueType::staticMetaObject;
        }

        return nullptr;
    }

    bool init(int type, QVariant &dst) {
        if (type == qMetaTypeId<fixed32>()) {
            qDebug() << "init: " << type;
            dst.setValue<fixed32>(0);
            return true;
        }

        return false;
    }

    bool create(int type, int argc, const void *argv[], QVariant *v) override {
        if (type == qMetaTypeId<fixed32>()) {
            if (argc == 1) {
                const int *value = reinterpret_cast<const int*>(argv[0]);
                qDebug() << "create: " << type << "value" << *value;
                *v = QVariant::fromValue(fixed32(*value));
                return true;
            }
        }

        return false;
    }

    bool createFromString(int type, const QString &s, void *data, size_t dataSize) override {
        bool ok = false;
        if (type == qMetaTypeId<fixed32>()) {
            Q_ASSERT(dataSize >= sizeof(fixed32));
            fixed32 *t = reinterpret_cast<fixed32 *>(data);
            qDebug() << "createFromString: " << type << "value" << *t;
            new (t) fixed32(s.toUInt(&ok));
            return ok;
        }

        return false;
    }

    bool createStringFrom(int type, const void *data, QString *s) override {
        if (type == qMetaTypeId<fixed32>()) {
            qDebug() << "createStringFrom: " << type;
            const fixed32 *t = reinterpret_cast<const fixed32 *>(data);
            new (s) QString(QString::number(t->_t));
            return true;
        }

        return false;
    }

    bool variantFromString(const QString &s, QVariant *v) override {
        bool ok = false;
        fixed32 t = fixed32(s.toUInt(&ok));
        if (ok) {
            qDebug() << "variantFromString1: ";
            *v = QVariant::fromValue(t);
            return ok;
        }

        return false;
    }

    bool variantFromString(int type, const QString &s, QVariant *v) override {
        bool ok = false;

        if (type == qMetaTypeId<fixed32>()) {
            qDebug() << "variantFromString: " << type;
            fixed32 t = fixed32(s.toUInt(&ok));
            if (ok) {
                *v = QVariant::fromValue(t);
            }
            return ok;
        }

        return false;
    }

//    bool variantFromJsObject(int type, QQmlV4Handle object, QV4::ExecutionEngine *v4, QVariant *v) override {
//        QV4::Scope scope(v4);
//        QV4::ScopedObject obj(scope, object);
//        if (type == qMetaTypeId<fixed32>) {
//            obj.ge
//        }
//    }

    bool equal(int type, const void *lhs, const QVariant &rhs) override {
        if (type == qMetaTypeId<fixed32>()) {
            return *(reinterpret_cast<const fixed32 *>(lhs)) == rhs.value<fixed32>();
        }

        return false;
    }

    bool store(int type, const void *src, void *dst, size_t dstSize) override {
        if (type == qMetaTypeId<fixed32>()) {
            qDebug() << "store: " << type;
            Q_ASSERT(dstSize >= sizeof(fixed32));
            const fixed32 *srcT = reinterpret_cast<const fixed32 *>(src);
            fixed32 *dstT = reinterpret_cast<fixed32 *>(dst);
            new (dstT) fixed32(*srcT);
            return true;
        }
        return false;
    }

    bool read(const QVariant &src, void *dst, int dstType) override {
        bool ok;
        if (dstType == qMetaTypeId<fixed32>()) {
            qDebug() << "read: " << dstType;
            fixed32 *dstT = reinterpret_cast<fixed32 *>(dst);
            *dstT = src.toUInt(&ok);
            if (!ok) {
                *dstT = src.value<fixed32>();
            }
            return true;
        }

        return false;
    }

    bool write(int type, const void *src, QVariant &dst) override {
        if (type == qMetaTypeId<fixed32>()) {
            qDebug() << "write: " << type;
            return true;
        }
        return false;
    }
};

QString QtProtobufFixed32ValueType::toString() const
{
    return QString::number(v._t);
}


void registerValueTypes() {
    QQml_addValueTypeProvider(QtProtobufValueTypeProvider::instance());
    QQmlValueTypeFactory::registerValueTypes("QtProtobuf", 1, 0);
}

void releaseValueTypes() {
    QQml_removeValueTypeProvider(QtProtobufValueTypeProvider::instance());
}

}
