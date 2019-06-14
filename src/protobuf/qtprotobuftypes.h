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

#include <QList>
#include <QMetaType>

#include <unordered_map>

namespace qtprotobuf {
/*!
*  \addtogroup QtProtobuf
*  \{
*/

/*!
 * \brief The WireTypes enumeration reflects protobuf default wiretypes
 *
 * \details Look at https://developers.google.com/protocol-buffers/docs/encoding for details.
 *
 */
enum WireTypes {
    UnknownWireType = -1, //!< Invalid wire type
    Varint = 0, //!< int32, int64, uint32, uint64, sint32, sint64, bool, enum
    Fixed64 = 1, //!< fixed64, sfixed64, double
    LengthDelimited = 2, //!< string, bytes, embedded messages, packed repeated fields
    StartGroup = 3, //!< groups \deprecated Is deprecated in proto syntax 3. Not supported by QtProtobuf
    EndGroup = 4, //!< groups \deprecated Is deprecated in proto syntax 3. Not supported by QtProtobuf
    Fixed32 = 5 //!< fixed32, sfixed32, float
};

//! \private
using QProtobufPropertyOrdering = std::unordered_map<int, int>;

/*!
 * \private
 *
 * \brief the transparent class is template to define set of extra types for integral types, to cover protobuf needs
 */
template<typename T, int = 0>
struct transparent {
    transparent(T t = T()) : _t(t){}
    T _t;
    operator T &(){ return _t; }
    operator T() const { return _t; }
    transparent &operator =(const T &t) { _t = t; return *this; }

    static T toType(transparent t) { return t._t; }
    static transparent fromType(T _t) { return transparent(_t); }

    static QString toString(transparent t) { return QString::number(t._t); }
};

/*!
 * \brief int32 signed 32-bit integer
 *
 * \details int32 is regular signed 32-bit integer that is represented in protobuf as variable size integer
 * aka WireTypes::Varint without modificators
 */
using int32 = transparent<int32_t>;

/*!
 * \brief int64 signed 64-bit integer
 *
 * \details int64 is regular signed 64-bit integer that is represented in protobuf as variable size integer
 * aka WireTypes::Varint without modificators
 */
using int64 = transparent<int64_t>;

/*!
 * \brief uint32 unsigned 32-bit integer
 *
 * \details uint32 is unsigned 32-bit integer that is represented in protobuf as variable size integer
 * aka WireTypes::Varint without modificators
 */
using uint32 = uint32_t;

/*!
 * \brief uint64 unsigned 64-bit integer
 *
 * \details uint64 is unsigned 64-bit integer that is represented in protobuf as variable size integer
 * aka WireTypes::Varint without modificators
 */
using uint64 = uint64_t;

/*!
 * \brief sint32 signed 32-bit ZigZag integer
 *
 * \details sint32 is 32-bit integer with forced sign marker that is represented in protobuf as variable size integer
 * aka WireTypes::Varint. sint32 type serialized using ZigZag convertion to reduce size of negative numbers.
 *
 * \see https://developers.google.com/protocol-buffers/docs/encoding#signed-integers for details.
 */
using sint32 = int32_t;

/*!
 * \brief sint64 signed 64-bit ZigZag integer
 *
 * \details sint64 is 64-bit integer with forced sign marker that is represented in protobuf as variable size integer
 * aka WireTypes::Varint. sint64 type serialized using ZigZag convertion to reduce size of negative numbers.
 *
 * \see https://developers.google.com/protocol-buffers/docs/encoding#signed-integers for details.
 */
using sint64 = int64_t;

/*!
 * \brief fixed32 unsigned 32-bit fixed size integer
 *
 * \details fixed32 is unsigned 32-bit integer that is represented in protobuf as fixed size 32-bit field
 * aka WireTypes::Fixed32
 */
using fixed32 = transparent<uint32_t, 1>;

/*!
 * \brief fixed64 unsigned 64-bit fixed size integer
 *
 * \details fixed64 is unsigned 64-bit integer that is represented in protobuf as fixed size 64-bit field
 * aka WireTypes::Fixed64
 */
using fixed64 = transparent<uint64_t, 1>;

/*!
 * \brief fixed32 signed 32-bit fixed size integer
 *
 * \details fixed32 is signed 32-bit integer that is represented in protobuf as fixed size 32-bit field
 * aka WireTypes::Fixed32
 */
using sfixed32 = transparent<int32_t, 1>;

/*!
 * \brief fixed64 signed 64-bit fixed size integer
 *
 * \details fixed64 is signed 64-bit integer that is represented in protobuf as fixed size 64-bit field
 * aka WireTypes::Fixed64
 */
using sfixed64 = transparent<int64_t, 1>;

/*!
 * \brief alias for list of qtprotobuf::int32
 */
using int32List = QList<int32>;

/*!
 * \brief alias for list of qtprotobuf::int64
 */
using int64List = QList<int64>;

/*!
 * \brief alias for list of qtprotobuf::uint32
 */
using uint32List = QList<uint32>;

/*!
 * \brief alias for list of qtprotobuf::uint64
 */
using uint64List = QList<uint64>;

/*!
 * \brief alias for list of qtprotobuf::sint32
 */
using sint32List = QList<sint32>;

/*!
 * \brief alias for list of qtprotobuf::sint64
 */
using sint64List = QList<sint64>;

/*!
 * \brief alias for list of qtprotobuf::fixed32
 */
using fixed32List = QList<fixed32>;

/*!
 * \brief alias for list of qtprotobuf::fixed64
 */
using fixed64List = QList<fixed64>;

/*!
 * \brief alias for list of qtprotobuf::sfixed32
 */
using sfixed32List = QList<sfixed32>;

/*!
 * \brief alias for list of qtprotobuf::sfixed64
 */
using sfixed64List = QList<sfixed64>;

/*!
 * \brief alias for list of qtprotobuf::float
 */
using FloatList = QList<float>;

/*!
 * \brief alias for list of qtprotobuf::double
 */
using DoubleList = QList<double>;

/*! \} */
}

Q_DECLARE_METATYPE(qtprotobuf::int32)
Q_DECLARE_METATYPE(qtprotobuf::int64)
Q_DECLARE_METATYPE(qtprotobuf::sint32)
Q_DECLARE_METATYPE(qtprotobuf::sint64)
Q_DECLARE_METATYPE(qtprotobuf::uint32)
Q_DECLARE_METATYPE(qtprotobuf::uint64)
Q_DECLARE_METATYPE(qtprotobuf::fixed32)
Q_DECLARE_METATYPE(qtprotobuf::fixed64)
Q_DECLARE_METATYPE(qtprotobuf::sfixed32)
Q_DECLARE_METATYPE(qtprotobuf::sfixed64)

Q_DECLARE_METATYPE(qtprotobuf::int32List)
Q_DECLARE_METATYPE(qtprotobuf::int64List)
Q_DECLARE_METATYPE(qtprotobuf::sint32List)
Q_DECLARE_METATYPE(qtprotobuf::sint64List)
Q_DECLARE_METATYPE(qtprotobuf::uint32List)
Q_DECLARE_METATYPE(qtprotobuf::uint64List)
Q_DECLARE_METATYPE(qtprotobuf::fixed32List)
Q_DECLARE_METATYPE(qtprotobuf::fixed64List)
Q_DECLARE_METATYPE(qtprotobuf::sfixed32List)
Q_DECLARE_METATYPE(qtprotobuf::sfixed64List)

Q_DECLARE_METATYPE(qtprotobuf::FloatList)
Q_DECLARE_METATYPE(qtprotobuf::DoubleList)

//! \private
template<>
struct std::make_unsigned<qtprotobuf::int32> { typedef typename std::make_unsigned<decltype(qtprotobuf::int32::_t)>::type type; };
//! \private
template<>
struct std::make_unsigned<qtprotobuf::int64> { typedef typename std::make_unsigned<decltype(qtprotobuf::int64::_t)>::type type; };
//! \private
template<>
struct std::make_unsigned<qtprotobuf::fixed32> { typedef typename std::make_unsigned<decltype(qtprotobuf::fixed32::_t)>::type type; };
//! \private
template<>
struct std::make_unsigned<qtprotobuf::fixed64> { typedef typename std::make_unsigned<decltype(qtprotobuf::fixed64::_t)>::type type; };
//! \private
template<>
struct std::make_unsigned<qtprotobuf::sfixed32> { typedef typename std::make_unsigned<decltype(qtprotobuf::sfixed32::_t)>::type type; };
//! \private
template<>
struct std::make_unsigned<qtprotobuf::sfixed64> { typedef typename std::make_unsigned<decltype(qtprotobuf::sfixed64::_t)>::type type; };

//! \private
template<>
struct std::make_signed<qtprotobuf::int32> { typedef typename std::make_signed<decltype(qtprotobuf::int32::_t)>::type type; };
//! \private
template<>
struct std::make_signed<qtprotobuf::int64> { typedef typename std::make_signed<decltype(qtprotobuf::int64::_t)>::type type; };
//! \private
template<>
struct std::make_signed<qtprotobuf::fixed32> { typedef typename std::make_signed<decltype(qtprotobuf::fixed32::_t)>::type type; };
//! \private
template<>
struct std::make_signed<qtprotobuf::fixed64> { typedef typename std::make_signed<decltype(qtprotobuf::fixed64::_t)>::type type; };
//! \private
template<>
struct std::make_signed<qtprotobuf::sfixed32> { typedef typename std::make_signed<decltype(qtprotobuf::sfixed32::_t)>::type type; };
//! \private
template<>
struct std::make_signed<qtprotobuf::sfixed64> { typedef typename std::make_signed<decltype(qtprotobuf::sfixed64::_t)>::type type; };

//! \private
template<>
struct std::is_signed<qtprotobuf::int32> : public is_signed<decltype(qtprotobuf::int32::_t)> {};
//! \private
template<>
struct std::is_signed<qtprotobuf::int64> : public is_signed<decltype(qtprotobuf::int64::_t)> {};
//! \private
template<>
struct std::is_signed<qtprotobuf::fixed32> : public is_signed<decltype(qtprotobuf::fixed32::_t)> {};
//! \private
template<>
struct std::is_signed<qtprotobuf::fixed64> : public is_signed<decltype(qtprotobuf::fixed64::_t)> {};
//! \private
template<>
struct std::is_signed<qtprotobuf::sfixed32> : public is_signed<decltype(qtprotobuf::sfixed32::_t)> {};
//! \private
template<>
struct std::is_signed<qtprotobuf::sfixed64> : public is_signed<decltype(qtprotobuf::sfixed64::_t)> {};
