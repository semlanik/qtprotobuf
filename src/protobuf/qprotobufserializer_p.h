/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>, Viktor Kopp <vifactor@gmail.com>
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

#include <QString>
#include <QByteArray>

#include "qprotobufselfcheckiterator.h"
#include "qtprotobuftypes.h"
#include "qtprotobuflogging.h"
#include "qabstractprotobufserializer.h"

namespace QtProtobuf {

/*!
 * \private
 *
 * \brief The QProtobufSerializerPrivate class
 */
class QProtobufSerializer;

class QProtobufSerializerPrivate final
{
    Q_DISABLE_COPY_MOVE(QProtobufSerializerPrivate)
public:

    /*!
     * \brief Serializer is interface function for serialize method
     */
    using Serializer = std::function<QByteArray(const QVariant &, int &)>;
    /*!
     * \brief Deserializer is interface function for deserialize method
     */
    using Deserializer = void(*)(QProtobufSelfcheckIterator &, QVariant &);

    /*!
     * \brief SerializationHandlers contains set of objects that required for class serializaion/deserialization
     */
    struct SerializationHandlers {
        Serializer serializer; /*!< serializer assigned to class */
        Deserializer deserializer;/*!< deserializer assigned to class */
        WireTypes type;/*!< Serialization WireType */
    };

    using SerializerRegistry = std::unordered_map<int/*metatypeid*/, SerializationHandlers>;

    QProtobufSerializerPrivate(QProtobufSerializer *q);
    ~QProtobufSerializerPrivate() = default;
    //###########################################################################
    //                               Serializers
    //###########################################################################

    template <typename V,
              typename std::enable_if_t<std::is_integral<V>::value
                                        && std::is_unsigned<V>::value, int> = 0>
    static QByteArray serializeVarintCommon(const V &value) {
        qProtoDebug() << __func__ << "value" << value;
        V varint = value;
        QByteArray result;

        while (varint != 0) {
            //Put 7 bits to result buffer and mark as "not last" (0b10000000)
            result.append((varint & 0b01111111) | 0b10000000);
            //Divide values to chunks of 7 bits and move to next chunk
            varint >>= 7;
        }

        if (result.isEmpty()) {
            result.append('\0');
        }

        result.data()[result.size() - 1] &= ~0b10000000;
        return result;
    }

    //---------------Integral and floating point types serializers---------------
    /*!
     * \brief Serialization of fixed-length primitive types
     *
     * Natural layout of bits is used: value is encoded in a byte array same way as it is located in memory
     *
     * \param[in] value Value to serialize
     * \param[out] outFieldIndex Index of the value in parent structure (ignored)
     * \return Byte array with value encoded
     */
    template <typename V,
              typename std::enable_if_t<std::is_floating_point<V>::value, int> = 0>
    static QByteArray serializeBasic(const V &value, int &/*outFieldIndex*/) {
        qProtoDebug() << __func__ << "value" << value;

        //Reserve required number of bytes
        QByteArray result(sizeof(V), '\0');
        *reinterpret_cast<V *>(result.data()) = value;
        return result;
    }

    /*!
     * \brief Serialization of fixed length integral types
     *
     * \details Natural layout of bits is employed
     *
     * \param[in] value Value to serialize
     * \param[out] outFieldIndex Index of the value in parent structure (ignored)
     * \return Byte array with value encoded
     */
    template <typename V,
              typename std::enable_if_t<std::is_same<V, fixed32>::value
                                        || std::is_same<V, fixed64>::value
                                        || std::is_same<V, sfixed32>::value
                                        || std::is_same<V, sfixed64>::value, int> = 0>
    static QByteArray serializeBasic(const V &value, int &/*outFieldIndex*/) {
        qProtoDebug() << __func__ << "value" << value;

        //Reserve required number of bytes
        QByteArray result(sizeof(V), '\0');
        *reinterpret_cast<V *>(result.data()) = value;
        return result;
    }

    /*!
     *\brief Serialization of signed integral types
     *
     * Use <a href="https://developers.google.com/protocol-buffers/docs/encoding">ZigZag encoding</a> first,
     * then apply serialization as for unsigned integral types
     * \see serializeBasic\<typename V, typename std::enable_if_t\<std::is_integral\<V\>::value && std::is_unsigned\<V\>::value, int\> = 0\>(V, int)
     *
     * \param[in] value Value to serialize
     * \param[out] outFieldIndex Index of the value in parent structure
     * \return Byte array with value encoded
     */
    template <typename V,
              typename std::enable_if_t<std::is_integral<V>::value
                                        && std::is_signed<V>::value, int> = 0>
    static QByteArray serializeBasic(const V &value, int &outFieldIndex) {
        qProtoDebug() << __func__ << "value" << value;
        using UV = typename std::make_unsigned<V>::type;
        UV uValue = 0;

        //Use ZigZag convertion first and apply unsigned variant next
        V zigZagValue = (value << 1) ^ (value >> (sizeof(UV) * 8 - 1));
        uValue = static_cast<UV>(zigZagValue);
        return serializeBasic(uValue, outFieldIndex);
    }

    template <typename V,
              typename std::enable_if_t<std::is_same<V, int32>::value
                                        || std::is_same<V, int64>::value, int> = 0>
    static QByteArray serializeBasic(const V &value, int &outFieldIndex) {
        qProtoDebug() << __func__ << "value" << value;
        using UV = typename std::make_unsigned<V>::type;
        return serializeBasic(static_cast<UV>(value), outFieldIndex);
    }

    /*!
    *\brief Serialization of unsigned integral types
    *
    * Use <a href="https://developers.google.com/protocol-buffers/docs/encoding">Varint encoding</a>:
    * "Varints are a method of serializing integers using one or more bytes. Smaller numbers
    * [regardless its type] take a smaller number of bytes."
    *
    * \param[in] value Value to serialize
    * \param[out] outFieldIndex Index of the value in parent structure
    * \return Byte array with value encoded
    */
    template <typename V,
              typename std::enable_if_t<std::is_integral<V>::value
                                        && std::is_unsigned<V>::value, int> = 0>
    static QByteArray serializeBasic(const V &value, int &outFieldIndex) {
        qProtoDebug() << __func__ << "value" << value;
        V varint = value;
        QByteArray result;

        while (varint != 0) {
            //Put 7 bits to result buffer and mark as "not last" (0b10000000)
            result.append((varint & 0b01111111) | 0b10000000);
            //Divide values to chunks of 7 bits and move to next chunk
            varint >>= 7;
        }

        // Invalidate field index in case if serialized result is empty
        // NOTE: the field will not be sent if its index is equal to NotUsedFieldIndex
        if (result.size() == 0) {
            outFieldIndex = QtProtobufPrivate::NotUsedFieldIndex;
        } else {
            //Mark last chunk as last by clearing last bit
            result.data()[result.size() - 1] &= ~0b10000000;
        }
        return result;
    }

    //------------------QString and QByteArray types serializers-----------------
    template <typename V,
              typename std::enable_if_t<std::is_same<V, QString>::value, int> = 0>
    static QByteArray serializeBasic(const V &value, int &/*outFieldIndex*/) {
        return serializeLengthDelimited(value.toUtf8());
    }

    template <typename V,
              typename std::enable_if_t<std::is_same<V, QByteArray>::value, int> = 0>
    static QByteArray serializeBasic(const V &value, int &/*outFieldIndex*/) {
        return serializeLengthDelimited(value);
    }

    //--------------------------List types serializers---------------------------
    template<typename V,
             typename std::enable_if_t<!(std::is_same<V, QString>::value
                                       || std::is_same<V, QByteArray>::value
                                       || std::is_base_of<QObject, V>::value), int> = 0>
    static QByteArray serializeListType(const QList<V> &listValue, int &outFieldIndex) {
        qProtoDebug() << __func__ << "listValue.count" << listValue.count() << "outFiledIndex" << outFieldIndex;

        if (listValue.count() <= 0) {
            outFieldIndex = QtProtobufPrivate::NotUsedFieldIndex;
            return QByteArray();
        }

        int empty = QtProtobufPrivate::NotUsedFieldIndex;
        QByteArray serializedList;
        for (auto &value : listValue) {
            serializedList.append(serializeBasic<V>(value, empty));
        }
        //If internal field type is not LengthDelimited, exact amount of fields to be specified
        serializedList = prependLengthDelimitedSize(serializedList);
        return serializedList;
    }

    template<typename V,
             typename std::enable_if_t<std::is_same<V, QString>::value, int> = 0>
    static QByteArray serializeListType(const QList<V> &listValue, int &outFieldIndex) {
        qProtoDebug() << __func__ << "listValue.count" << listValue.count() << "outFiledIndex" << outFieldIndex;

        if (listValue.count() <= 0) {
            outFieldIndex = QtProtobufPrivate::NotUsedFieldIndex;
            return QByteArray();
        }

        QByteArray serializedList;
        for (auto &value : listValue) {
            serializedList.append(QProtobufSerializerPrivate::encodeHeader(outFieldIndex, LengthDelimited));
            serializedList.append(serializeLengthDelimited(value.toUtf8()));
        }

        outFieldIndex = QtProtobufPrivate::NotUsedFieldIndex;
        return serializedList;
    }

    template<typename V,
             typename std::enable_if_t<std::is_same<V, QByteArray>::value, int> = 0>
    static QByteArray serializeListType(const QList<V> &listValue, int &outFieldIndex) {
        qProtoDebug() << __func__ << "listValue.count" << listValue.count() << "outFiledIndex" << outFieldIndex;

        if (listValue.count() <= 0) {
            outFieldIndex = QtProtobufPrivate::NotUsedFieldIndex;
            return QByteArray();
        }

        QByteArray serializedList;
        for (auto &value : listValue) {
            serializedList.append(QProtobufSerializerPrivate::encodeHeader(outFieldIndex, LengthDelimited));
            serializedList.append(serializeLengthDelimited(value));
        }

        outFieldIndex = QtProtobufPrivate::NotUsedFieldIndex;
        return serializedList;
    }

    //###########################################################################
    //                               Deserializers
    //###########################################################################
    template <typename V,
              typename std::enable_if_t<std::is_integral<V>::value
                                        && std::is_unsigned<V>::value, int> = 0>
    static V deserializeVarintCommon(QProtobufSelfcheckIterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        V value = 0;
        int k = 0;
        while (true) {
            uint64_t byte = static_cast<uint64_t>(*it);
            value += (byte & 0b01111111) << k;
            k += 7;
            if (((*it) & 0b10000000) == 0) {
                break;
            }
            ++it;
        }
        ++it;
        return value;
    }

    //-------------Integral and floating point types deserializers---------------
    template <typename V,
              typename std::enable_if_t<std::is_floating_point<V>::value
                                        || std::is_same<V, fixed32>::value
                                        || std::is_same<V, fixed64>::value
                                        || std::is_same<V, sfixed32>::value
                                        || std::is_same<V, sfixed64>::value, int> = 0>
    static void deserializeBasic(QProtobufSelfcheckIterator &it, QVariant &variantValue) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        variantValue = QVariant::fromValue(*(V *)((QByteArray::const_iterator&)it));
        it += sizeof(V);
    }

    template <typename V,
              typename std::enable_if_t<std::is_integral<V>::value
                                        && std::is_unsigned<V>::value, int> = 0>
    static void deserializeBasic(QProtobufSelfcheckIterator &it, QVariant &variantValue) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        variantValue = QVariant::fromValue(deserializeVarintCommon<V>(it));
    }

    template <typename V,
              typename std::enable_if_t<std::is_integral<V>::value
                                        && std::is_signed<V>::value,int> = 0>
    static void deserializeBasic(QProtobufSelfcheckIterator &it, QVariant &variantValue) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);
        using  UV = typename std::make_unsigned<V>::type;
        UV unsignedValue = deserializeVarintCommon<UV>(it);
        V value = (unsignedValue >> 1) ^ (-1 * (unsignedValue & 1));
        variantValue = QVariant::fromValue<V>(value);
    }

    template <typename V,
              typename std::enable_if_t<std::is_same<int32, V>::value
                                        || std::is_same<int64, V>::value, int> = 0>
    static void deserializeBasic(QProtobufSelfcheckIterator &it, QVariant &variantValue) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);
        using  UV = typename std::make_unsigned<V>::type;
        UV unsignedValue = deserializeVarintCommon<UV>(it);
        V value = static_cast<V>(unsignedValue);
        variantValue = QVariant::fromValue(value);
    }

    //-----------------QString and QByteArray types deserializers----------------
    template <typename V,
              typename std::enable_if_t<std::is_same<QByteArray, V>::value, int> = 0>
    static void deserializeBasic(QProtobufSelfcheckIterator &it, QVariant &variantValue) {
        variantValue = QVariant::fromValue(deserializeLengthDelimited(it));
    }

    template <typename V,
              typename std::enable_if_t<std::is_same<QString, V>::value, int> = 0>
    static void deserializeBasic(QProtobufSelfcheckIterator &it, QVariant &variantValue) {
        variantValue = QVariant::fromValue(QString::fromUtf8(deserializeLengthDelimited(it)));
    }

    //-------------------------List types deserializers--------------------------
    template <typename V,
              typename std::enable_if_t<std::is_same<V, QByteArray>::value, int> = 0>
    static void deserializeList(QProtobufSelfcheckIterator &it, QVariant &previousValue) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        QByteArrayList list = previousValue.value<QByteArrayList>();
        list.append(deserializeLengthDelimited(it));
        previousValue.setValue(list);
    }

    template <typename V,
              typename std::enable_if_t<std::is_same<V, QString>::value, int> = 0>
    static void deserializeList(QProtobufSelfcheckIterator &it, QVariant &previousValue) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        QStringList list = previousValue.value<QStringList>();
        QByteArray value = deserializeLengthDelimited(it);
        list.append(QString::fromUtf8(value));
        previousValue.setValue(list);
    }

    template <typename V,
              typename std::enable_if_t<!(std::is_same<V, QString>::value
                                        || std::is_same<V, QByteArray>::value
                                        || std::is_base_of<QObject, V>::value), int> = 0>
    static void deserializeList(QProtobufSelfcheckIterator &it, QVariant &previousValue) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        QList<V> out;
        unsigned int count = deserializeVarintCommon<uint32>(it);
        QProtobufSelfcheckIterator lastVarint = it + count;
        while (it != lastVarint) {
            QVariant variantValue;
            deserializeBasic<V>(it, variantValue);
            out.append(variantValue.value<V>());
        }
        previousValue.setValue(out);
    }

    //###########################################################################
    //                             Common functions
    //###########################################################################
    static QByteArray deserializeLengthDelimited(QProtobufSelfcheckIterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        unsigned int length = deserializeVarintCommon<uint32>(it);
        QByteArray result((QByteArray::const_iterator&)it, length); //TODO: it's possible to void buffeer copying by setupimg new "end of QByteArray";
        it += length;
        return result;
    }

    static QByteArray serializeLengthDelimited(const QByteArray &data) {
        qProtoDebug() << __func__ << "data.size" << data.size() << "data" << data.toHex();
        QByteArray result(data);
        //Varint serialize field size and apply result as starting point
        result = prependLengthDelimitedSize(result);
        return result;
    }

    static bool decodeHeader(QProtobufSelfcheckIterator &it, int &fieldIndex, WireTypes &wireType);
    static QByteArray encodeHeader(int fieldIndex, WireTypes wireType);

    /*!
     * \brief Gets length of a byte-array and prepends to it its serialized length value
     *      using the appropriate serialization algorithm
     *
     *
     * \param[in, out] serializedList Byte-array to be prepended
     */
    static QByteArray prependLengthDelimitedSize(const QByteArray &data)
    {
        return serializeVarintCommon<uint32_t>(data.size()) + data;
    }

    template <typename T,
    typename std::enable_if_t<!std::is_base_of<QObject, T>::value, int> = 0>
    static void wrapSerializer(const std::function<QByteArray(const T &, int &)> &s, const Deserializer &d, WireTypes type)
    {
        handlers[qMetaTypeId<T>()] = {
            [s](const QVariant &variantValue, int &fieldIndex) {
                if (variantValue.isNull()) {
                    return QByteArray();
                }
                const T& value = *(static_cast<const T *>(variantValue.data()));
                return s(value, fieldIndex);
            },
            d,
            type
        };
    }

    // this set of 3 methods is used to skip bytes corresponding to an unexpected property
    // in a serialized message met while the message being deserialized
    static int skipSerializedFieldBytes(QProtobufSelfcheckIterator &it, WireTypes type);
    static void skipVarint(QProtobufSelfcheckIterator &it);
    static void skipLengthDelimited(QProtobufSelfcheckIterator &it);

    QByteArray serializeProperty(const QVariant &propertyValue, const QProtobufMetaProperty &metaProperty);
    void deserializeProperty(QObject *object, QProtobufSelfcheckIterator &it, const QProtobufPropertyOrdering &propertyOrdering, const QMetaObject &metaObject);

    static SerializerRegistry handlers;
private:
    QProtobufSerializer *q_ptr;
};

//###########################################################################
//                             Common functions
//###########################################################################

/*! \brief Encode a property field index and its type into output bytes
 *
 * \details
 * Header byte
 *  Meaning    |  Field index  |  Type
 *  ---------- | ------------- | --------
 *  bit number | 7  6  5  4  3 | 2  1  0
 * \param fieldIndex The index of a property in parent object
 * \param wireType Serialization type used for the property with index @p fieldIndex
 *
 * \return Varint encoded fieldIndex and wireType
 */
inline QByteArray QProtobufSerializerPrivate::encodeHeader(int fieldIndex, WireTypes wireType)
{
    uint32_t header = (fieldIndex << 3) | wireType;
    return serializeVarintCommon<uint32_t>(header);
}

/*! \brief Decode a property field index and its serialization type from input bytes
 *
 * \param[in] Iterator that points to header with encoded field index and serialization type
 * \param[out] fieldIndex Decoded index of a property in parent object
 * \param[out] wireType Decoded serialization type used for the property with index @p fieldIndex
 *
 * \return true if both decoded wireType and fieldIndex have "allowed" values and false, otherwise
 */
inline bool QProtobufSerializerPrivate::decodeHeader(QProtobufSelfcheckIterator &it, int &fieldIndex, WireTypes &wireType)
{
    uint32_t header = deserializeVarintCommon<uint32_t>(it);
    wireType = static_cast<WireTypes>(header & 0b00000111);
    fieldIndex = header >> 3;

    constexpr int maxFieldIndex = (1 << 29) - 1;
    return fieldIndex <= maxFieldIndex && fieldIndex > 0 && (wireType == Varint
                                                             || wireType == Fixed64
                                                             || wireType == Fixed32
                                                             || wireType == LengthDelimited);
}

}
