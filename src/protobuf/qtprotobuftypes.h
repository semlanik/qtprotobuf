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

#pragma once //QtProtobufTypes

#include "qtprotobufglobal.h"

#include <QList>
#include <QMetaType>

#include <unordered_map>
#include <functional>
#include <list>

namespace QtProtobuf {

/*!
 * \ingroup QtProtobuf
 * \brief The WireTypes enumeration reflects protobuf default wiretypes
 *
 * \see https://developers.google.com/protocol-buffers/docs/encoding for details.
 *
 */
enum WireTypes {
    UnknownWireType = -1, //!< Invalid wire type
    Varint = 0,           //!< int32, int64, uint32, uint64, sint32, sint64, bool, enum
    Fixed64 = 1,          //!< fixed64, sfixed64, double
    LengthDelimited = 2,  //!< string, bytes, embedded messages, packed repeated fields
    StartGroup = 3,       //!< groups \deprecated Is deprecated in proto syntax 3. Not supported by QtProtobuf
    EndGroup = 4,         //!< groups \deprecated Is deprecated in proto syntax 3. Not supported by QtProtobuf
    Fixed32 = 5           //!< fixed32, sfixed32, float
};

//! \private
using QProtobufPropertyOrdering = std::unordered_map<int, int>;

/*!
 * \private
 * \ingroup QtProtobuf
 * \brief the transparent class is template to define set of extra types for integral types, to cover protobuf needs
 */
template<typename T, int = 0>
struct transparent {
    transparent(T t = T()) : _t(t) {}
    T _t;
    operator T &() { return _t; }
    operator T() const { return _t; }
    transparent &operator =(const T &t) { _t = t; return *this; }

    static T toType(transparent t) { return t._t; }
    static transparent fromType(T _t) { return transparent(_t); }

    static QString toString(transparent t) { return QString::number(t._t); }
};

/*!
 * \brief int32 signed 32-bit integer
 * \ingroup QtProtobuf
 * \details int32 is regular signed 32-bit integer that is represented in protobuf as variable size integer
 *          aka WireTypes::Varint without modificators
 */
using int32 = transparent<int32_t>;

/*!
 * \brief int64 signed 64-bit integer
 *
 * \details int64 is regular signed 64-bit integer that is represented in protobuf as variable size integer
 *          aka WireTypes::Varint without modificators
 */
using int64 = transparent<int64_t>;

/*!
 * \brief uint32 unsigned 32-bit integer
 * \ingroup QtProtobuf
 * \details uint32 is unsigned 32-bit integer that is represented in protobuf as variable size integer
 *          aka WireTypes::Varint without modificators
 */
using uint32 = uint32_t;

/*!
 * \brief uint64 unsigned 64-bit integer
 * \ingroup QtProtobuf
 * \details uint64 is unsigned 64-bit integer that is represented in protobuf as variable size integer
 *          aka WireTypes::Varint without modificators
 */
using uint64 = uint64_t;

/*!
 * \brief sint32 signed 32-bit ZigZag integer
 * \ingroup QtProtobuf
 * \details sint32 is 32-bit integer with forced sign marker that is represented in protobuf as variable size integer
 *          aka WireTypes::Varint. sint32 type serialized using ZigZag convertion to reduce size of negative numbers.
 *
 * \see https://developers.google.com/protocol-buffers/docs/encoding#signed-integers for details.
 */
using sint32 = int32_t;

/*!
 * \brief sint64 signed 64-bit ZigZag integer
 * \ingroup QtProtobuf
 * \details sint64 is 64-bit integer with forced sign marker that is represented in protobuf as variable size integer
 *          aka WireTypes::Varint. sint64 type serialized using ZigZag convertion to reduce size of negative numbers.
 *
 * \see https://developers.google.com/protocol-buffers/docs/encoding#signed-integers for details.
 */
using sint64 = int64_t;

/*!
 * \brief fixed32 unsigned 32-bit fixed size integer
 * \ingroup QtProtobuf
 * \details fixed32 is unsigned 32-bit integer that is represented in protobuf as fixed size 32-bit field
 *          aka WireTypes::Fixed32
 */
using fixed32 = transparent<uint32_t, 1>;

/*!
 * \brief fixed64 unsigned 64-bit fixed size integer
 * \ingroup QtProtobuf
 * \details fixed64 is unsigned 64-bit integer that is represented in protobuf as fixed size 64-bit field
 *          aka WireTypes::Fixed64
 */
using fixed64 = transparent<uint64_t, 1>;

/*!
 * \brief fixed32 signed 32-bit fixed size integer
 * \ingroup QtProtobuf
 * \details fixed32 is signed 32-bit integer that is represented in protobuf as fixed size 32-bit field
 *          aka WireTypes::Fixed32
 */
using sfixed32 = transparent<int32_t, 1>;

/*!
 * \brief fixed64 signed 64-bit fixed size integer
 * \ingroup QtProtobuf
 * \details fixed64 is signed 64-bit integer that is represented in protobuf as fixed size 64-bit field
 * aka WireTypes::Fixed64
 */
using sfixed64 = transparent<int64_t, 1>;

/*!
 * \ingroup QtProtobuf
 * \brief alias for list of QtProtobuf::int32
 */
using int32List = QList<int32>;

/*!
 * \ingroup QtProtobuf
 * \brief alias for list of QtProtobuf::int64
 */
using int64List = QList<int64>;

/*!
 * \ingroup QtProtobuf
 * \brief alias for list of QtProtobuf::uint32
 */
using uint32List = QList<uint32>;

/*!
 * \ingroup QtProtobuf
 * \brief alias for list of QtProtobuf::uint64
 */
using uint64List = QList<uint64>;

/*!
 * \ingroup QtProtobuf
 * \brief alias for list of QtProtobuf::sint32
 */
using sint32List = QList<sint32>;

/*!
 * \ingroup QtProtobuf
 * \brief alias for list of QtProtobuf::sint64
 */
using sint64List = QList<sint64>;

/*!
 * \ingroup QtProtobuf
 * \brief alias for list of QtProtobuf::fixed32
 */
using fixed32List = QList<fixed32>;

/*!
 * \ingroup QtProtobuf
 * \brief alias for list of QtProtobuf::fixed64
 */
using fixed64List = QList<fixed64>;

/*!
 * \ingroup QtProtobuf
 * \brief alias for list of QtProtobuf::sfixed32
 */
using sfixed32List = QList<sfixed32>;

/*!
 * \ingroup QtProtobuf
 * \brief alias for list of QtProtobuf::sfixed64
 */
using sfixed64List = QList<sfixed64>;

/*!
 * \ingroup QtProtobuf
 * \brief alias for list of QtProtobuf::float
 */
using FloatList = QList<float>;

/*!
 * \ingroup QtProtobuf
 * \brief alias for list of QtProtobuf::double
 */
using DoubleList = QList<double>;

/*!
 * \ingroup QtProtobuf
 * \brief qRegisterProtobufTypes
 * This method should be called in all applications that supposed to use QtProtobuf
 */
Q_PROTOBUF_EXPORT void qRegisterProtobufTypes();

/*! \} */

//!\private
using RegisterFunction = std::function<void(void)>;

//!\private
Q_PROTOBUF_EXPORT std::list<RegisterFunction>& registerFunctions();

//!\private
template <typename T>
struct ProtoTypeRegistrar {
    ProtoTypeRegistrar(RegisterFunction initializer) {
        registerFunctions().push_back(initializer);
    }
};

}

Q_DECLARE_METATYPE(QtProtobuf::int32)
Q_DECLARE_METATYPE(QtProtobuf::int64)
Q_DECLARE_METATYPE(QtProtobuf::sint32)
Q_DECLARE_METATYPE(QtProtobuf::sint64)
Q_DECLARE_METATYPE(QtProtobuf::uint32)
Q_DECLARE_METATYPE(QtProtobuf::uint64)
Q_DECLARE_METATYPE(QtProtobuf::fixed32)
Q_DECLARE_METATYPE(QtProtobuf::fixed64)
Q_DECLARE_METATYPE(QtProtobuf::sfixed32)
Q_DECLARE_METATYPE(QtProtobuf::sfixed64)

Q_DECLARE_METATYPE(QtProtobuf::int32List)
Q_DECLARE_METATYPE(QtProtobuf::int64List)
Q_DECLARE_METATYPE(QtProtobuf::sint32List)
Q_DECLARE_METATYPE(QtProtobuf::sint64List)
Q_DECLARE_METATYPE(QtProtobuf::uint32List)
Q_DECLARE_METATYPE(QtProtobuf::uint64List)
Q_DECLARE_METATYPE(QtProtobuf::fixed32List)
Q_DECLARE_METATYPE(QtProtobuf::fixed64List)
Q_DECLARE_METATYPE(QtProtobuf::sfixed32List)
Q_DECLARE_METATYPE(QtProtobuf::sfixed64List)

Q_DECLARE_METATYPE(QtProtobuf::FloatList)
Q_DECLARE_METATYPE(QtProtobuf::DoubleList)

namespace std {
//! \private
template<>
struct make_unsigned<QtProtobuf::int32> { typedef typename make_unsigned<decltype(QtProtobuf::int32::_t)>::type type; };
//! \private
template<>
struct make_unsigned<QtProtobuf::int64> { typedef typename make_unsigned<decltype(QtProtobuf::int64::_t)>::type type; };
//! \private
template<>
struct make_unsigned<QtProtobuf::fixed32> { typedef typename make_unsigned<decltype(QtProtobuf::fixed32::_t)>::type type; };
//! \private
template<>
struct make_unsigned<QtProtobuf::fixed64> { typedef typename make_unsigned<decltype(QtProtobuf::fixed64::_t)>::type type; };
//! \private
template<>
struct make_unsigned<QtProtobuf::sfixed32> { typedef typename make_unsigned<decltype(QtProtobuf::sfixed32::_t)>::type type; };
//! \private
template<>
struct make_unsigned<QtProtobuf::sfixed64> { typedef typename make_unsigned<decltype(QtProtobuf::sfixed64::_t)>::type type; };

//! \private
template<>
struct make_signed<QtProtobuf::int32> { typedef typename make_signed<decltype(QtProtobuf::int32::_t)>::type type; };
//! \private
template<>
struct make_signed<QtProtobuf::int64> { typedef typename make_signed<decltype(QtProtobuf::int64::_t)>::type type; };
//! \private
template<>
struct make_signed<QtProtobuf::fixed32> { typedef typename make_signed<decltype(QtProtobuf::fixed32::_t)>::type type; };
//! \private
template<>
struct make_signed<QtProtobuf::fixed64> { typedef typename make_signed<decltype(QtProtobuf::fixed64::_t)>::type type; };
//! \private
template<>
struct make_signed<QtProtobuf::sfixed32> { typedef typename make_signed<decltype(QtProtobuf::sfixed32::_t)>::type type; };
//! \private
template<>
struct make_signed<QtProtobuf::sfixed64> { typedef typename make_signed<decltype(QtProtobuf::sfixed64::_t)>::type type; };

//! \private
template<>
struct is_signed<QtProtobuf::int32> : public is_signed<decltype(QtProtobuf::int32::_t)> {};
//! \private
template<>
struct is_signed<QtProtobuf::int64> : public is_signed<decltype(QtProtobuf::int64::_t)> {};
//! \private
template<>
struct is_signed<QtProtobuf::fixed32> : public is_signed<decltype(QtProtobuf::fixed32::_t)> {};
//! \private
template<>
struct is_signed<QtProtobuf::fixed64> : public is_signed<decltype(QtProtobuf::fixed64::_t)> {};
//! \private
template<>
struct is_signed<QtProtobuf::sfixed32> : public is_signed<decltype(QtProtobuf::sfixed32::_t)> {};
//! \private
template<>
struct is_signed<QtProtobuf::sfixed64> : public is_signed<decltype(QtProtobuf::sfixed64::_t)> {};

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
template<> struct hash<QString> {
    std::size_t operator()(const QString &s) const {
        return std::hash<std::string>()(s.toStdString());
    }
};
#endif
}
