/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>, Viktor Kopp <vifactor@gmail.com>
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

#include <QString>
#include <QByteArray>

#include "qtprotobuftypes.h"
#include "qprotobufobject_p.h"

namespace qtprotobuf {

class QProtobufSerializerPrivate {
    QProtobufSerializerPrivate() = delete;
    ~QProtobufSerializerPrivate() = delete;
    Q_DISABLE_COPY(QProtobufSerializerPrivate)
    QProtobufSerializerPrivate(ProtobufObjectPrivate &&) = delete;
    QProtobufSerializerPrivate &operator =(QProtobufSerializerPrivate &&) = delete;
public:
    template <typename T,
              typename std::enable_if_t<!std::is_base_of<QObject, T>::value, int> = 0>
    static void wrapSerializer(const std::function<QByteArray(const T &, int &)> &s, const std::function<QVariant(SelfcheckIterator &)> &d, WireTypes type)
    {
        ProtobufObjectPrivate::serializers[qMetaTypeId<T>()] = {
            [s](const QVariant &value, int &fieldIndex) {
                return s(value.value<T>(), fieldIndex);
            },
            [d](SelfcheckIterator &it, QVariant & value){
                value = d(it);
            },
            type
        };
    }

    template <typename T,
              typename std::enable_if_t<!std::is_base_of<QObject, T>::value, int> = 0>
    static void wrapSerializer(const std::function<QByteArray(const T &, int &)> &s, const std::function<void(SelfcheckIterator &it, QVariant & value)> &d, WireTypes type)
    {
        ProtobufObjectPrivate::serializers[qMetaTypeId<T>()] = {
            [s](const QVariant &value, int &fieldIndex) {
                return s(value.value<T>(), fieldIndex);
            },
            d,
            type
        };
    }

    //----------------Serialize basic integral and floating point----------------
    /**
     * @brief Serialization of fixed-length primitive types
     *
     * Natural layout of bits is used: value is encoded in a byte array same way as it is located in memory
     *
     * @param[in] value Value to serialize
     * @param[out] outFieldIndex Index of the value in parent structure (ignored)
     * @return Byte array with value encoded
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

    /**
     * @brief Serialization of fixed length integral types
     *
     * @details Natural layout of bits is employed
     *
     * @param[in] value Value to serialize
     * @param[out] outFieldIndex Index of the value in parent structure (ignored)
     * @return Byte array with value encoded
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

    /**
     *@brief Serialization of signed integral types
     *
     * Use <a href="https://developers.google.com/protocol-buffers/docs/encoding">ZigZag encoding</a> first,
     * then apply serialization as for unsigned integral types
     * @see serializeBasic\<typename V, typename std::enable_if_t\<std::is_integral\<V\>::value && std::is_unsigned\<V\>::value, int\> = 0\>(V, int)
     *
     * @param[in] value Value to serialize
     * @param[out] outFieldIndex Index of the value in parent structure
     * @return Byte array with value encoded
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

    /**
    *@brief Serialization of unsigned integral types
    *
    * Use <a href="https://developers.google.com/protocol-buffers/docs/encoding">Varint encoding</a>:
    * "Varints are a method of serializing integers using one or more bytes. Smaller numbers
    * [regardless its type] take a smaller number of bytes."
    *
    * @param[in] value Value to serialize
    * @param[out] outFieldIndex Index of the value in parent structure
    * @return Byte array with value encoded
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
            outFieldIndex = NotUsedFieldIndex;
        } else {
            //Mark last chunk as last by clearing last bit
            result.data()[result.size() - 1] &= ~0b10000000;
        }
        return result;
    }

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

    //---------------Deserialize basic integral and floating point---------------
    template <typename V,
              typename std::enable_if_t<std::is_floating_point<V>::value
                                        || std::is_same<V, fixed32>::value
                                        || std::is_same<V, fixed64>::value
                                        || std::is_same<V, sfixed32>::value
                                        || std::is_same<V, sfixed64>::value, int> = 0>
    static QVariant deserializeBasic(SelfcheckIterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        QVariant newPropertyValue(QVariant::fromValue(*(V *)((QByteArray::const_iterator&)it)));
        it += sizeof(V);
        return newPropertyValue;
    }

    template <typename V,
              typename std::enable_if_t<std::is_integral<V>::value
                                        && std::is_unsigned<V>::value, int> = 0>
    static QVariant deserializeBasic(SelfcheckIterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        return QVariant::fromValue(deserializeVarintCommon<V>(it));
    }

    template <typename V,
              typename std::enable_if_t<std::is_integral<V>::value
                                        && std::is_signed<V>::value,int> = 0>
    static QVariant deserializeBasic(SelfcheckIterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);
        using  UV = typename std::make_unsigned<V>::type;
        UV unsignedValue = deserializeVarintCommon<UV>(it);
        V value = (unsignedValue >> 1) ^ (-1 * (unsignedValue & 1));
        return QVariant::fromValue<V>(value);
    }

    template <typename V,
              typename std::enable_if_t<std::is_same<int32, V>::value
                                        || std::is_same<int64, V>::value, int> = 0>
    static QVariant deserializeBasic(SelfcheckIterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);
        using  UV = typename std::make_unsigned<V>::type;
        UV unsignedValue = deserializeVarintCommon<UV>(it);
        V value = static_cast<V>(unsignedValue);
        return QVariant::fromValue(value);
    }

    template <typename V,
              typename std::enable_if_t<std::is_same<QByteArray, V>::value, int> = 0>
    static QVariant deserializeBasic(SelfcheckIterator &it) {
        return QVariant::fromValue(deserializeLengthDelimited(it));
    }

    template <typename V,
              typename std::enable_if_t<std::is_same<QString, V>::value, int> = 0>
    static QVariant deserializeBasic(SelfcheckIterator &it) {
        return QVariant::fromValue(QString::fromUtf8(deserializeLengthDelimited(it)));
    }

    template<typename V,
             typename std::enable_if_t<!(std::is_same<V, QString>::value
                                       || std::is_same<V, QByteArray>::value
                                       || std::is_base_of<QObject, V>::value), int> = 0>
    static QByteArray serializeListType(const QList<V> &listValue, int &outFieldIndex) {
        qProtoDebug() << __func__ << "listValue.count" << listValue.count() << "outFiledIndex" << outFieldIndex;

        if (listValue.count() <= 0) {
            outFieldIndex = NotUsedFieldIndex;
            return QByteArray();
        }

        int empty = NotUsedFieldIndex;
        QByteArray serializedList;
        for (auto &value : listValue) {
            serializedList.append(serializeBasic<V>(value, empty));
        }
        //If internal field type is not LengthDelimited, exact amount of fields to be specified
        ProtobufObjectPrivate::prependLengthDelimitedSize(serializedList);
        return serializedList;
    }

    template<typename V,
             typename std::enable_if_t<std::is_same<V, QString>::value, int> = 0>
    static QByteArray serializeListType(const QList<V> &listValue, int &outFieldIndex) {
        qProtoDebug() << __func__ << "listValue.count" << listValue.count() << "outFiledIndex" << outFieldIndex;

        if (listValue.count() <= 0) {
            outFieldIndex = NotUsedFieldIndex;
            return QByteArray();
        }

        QByteArray serializedList;
        for (auto &value : listValue) {
            serializedList.append(ProtobufObjectPrivate::encodeHeader(outFieldIndex, LengthDelimited));
            serializedList.append(serializeLengthDelimited(value.toUtf8()));
        }

        outFieldIndex = NotUsedFieldIndex;
        return serializedList;
    }

    template<typename V,
             typename std::enable_if_t<std::is_same<V, QByteArray>::value, int> = 0>
    static QByteArray serializeListType(const QList<V> &listValue, int &outFieldIndex) {
        qProtoDebug() << __func__ << "listValue.count" << listValue.count() << "outFiledIndex" << outFieldIndex;

        if (listValue.count() <= 0) {
            outFieldIndex = NotUsedFieldIndex;
            return QByteArray();
        }

        QByteArray serializedList;
        for (auto &value : listValue) {
            serializedList.append(ProtobufObjectPrivate::encodeHeader(outFieldIndex, LengthDelimited));
            serializedList.append(serializeLengthDelimited(value));
        }

        outFieldIndex = NotUsedFieldIndex;
        return serializedList;
    }

    template <typename V,
              typename std::enable_if_t<std::is_same<V, QByteArray>::value, int> = 0>
    static void deserializeList(SelfcheckIterator &it, QVariant &previous) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        QByteArrayList list = previous.value<QByteArrayList>();
        list.append(deserializeLengthDelimited(it));
        previous.setValue(list);
    }

    template <typename V,
              typename std::enable_if_t<std::is_same<V, QString>::value, int> = 0>
    static void deserializeList(SelfcheckIterator &it, QVariant &previous) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        QStringList list = previous.value<QStringList>();
        QByteArray value = deserializeLengthDelimited(it);
        list.append(QString::fromUtf8(value));
        previous.setValue(list);
    }

    template <typename V,
              typename std::enable_if_t<!(std::is_same<V, QString>::value
                                        || std::is_same<V, QByteArray>::value
                                        || std::is_base_of<QObject, V>::value), int> = 0>
    static void deserializeList(SelfcheckIterator &it, QVariant &previous) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        QList<V> out;
        unsigned int count = deserializeVarintCommon<uint32>(it);
        SelfcheckIterator lastVarint = it + count;
        while (it != lastVarint) {
            QVariant variant = deserializeBasic<V>(it);
            out.append(variant.value<V>());
        }
        previous.setValue(out);
    }

    static QByteArray deserializeLengthDelimited(SelfcheckIterator &it) {
        qProtoDebug() << __func__ << "currentByte:" << QString::number((*it), 16);

        unsigned int length = deserializeVarintCommon<uint32>(it);
        QByteArray result((QByteArray::const_iterator&)it, length);
        it += length;
        return result;
    }

    static QByteArray serializeLengthDelimited(const QByteArray &data) {
        qProtoDebug() << __func__ << "data.size" << data.size() << "data" << data.toHex();
        QByteArray result(data);
        //Varint serialize field size and apply result as starting point
        ProtobufObjectPrivate::prependLengthDelimitedSize(result);
        return result;
    }
};

}
