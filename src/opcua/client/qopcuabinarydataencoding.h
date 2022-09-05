// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUABINARYDATAENCODING_H
#define QOPCUABINARYDATAENCODING_H

#include <QtOpcUa/qopcuaapplicationrecorddatatype.h>
#include <QtOpcUa/qopcuaargument.h>
#include <QtOpcUa/qopcuaaxisinformation.h>
#include <QtOpcUa/qopcuacomplexnumber.h>
#include <QtOpcUa/qopcuadoublecomplexnumber.h>
#include <QtOpcUa/qopcuaeuinformation.h>
#include <QtOpcUa/qopcuaexpandednodeid.h>
#include <QtOpcUa/qopcuaextensionobject.h>
#include <QtOpcUa/qopcualocalizedtext.h>
#include <QtOpcUa/qopcuaqualifiedname.h>
#include <QtOpcUa/qopcuarange.h>
#include <QtOpcUa/qopcuatype.h>
#include <QtOpcUa/qopcuaxvalue.h>

#include <QtCore/qdatetime.h>
#include <QtCore/qendian.h>
#include <QtCore/qlist.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qtimezone.h>
#include <QtCore/quuid.h>

#include <limits>

QT_BEGIN_NAMESPACE

// This class implements a subset of the OPC UA Binary DataEncoding defined in OPC-UA part 6, 5.2.
class Q_OPCUA_EXPORT QOpcUaBinaryDataEncoding
{
public:

    QOpcUaBinaryDataEncoding(QByteArray *buffer);
    QOpcUaBinaryDataEncoding(QOpcUaExtensionObject &object);

    template <typename T, QOpcUa::Types OVERLAY = QOpcUa::Types::Undefined>
    T decode(bool &success);
    template <typename T, QOpcUa::Types OVERLAY = QOpcUa::Types::Undefined>
    QList<T> decodeArray(bool &success);

    template <typename T, QOpcUa::Types OVERLAY = QOpcUa::Types::Undefined>
    bool encode(const T &src);
    template <typename T, QOpcUa::Types OVERLAY = QOpcUa::Types::Undefined>
    bool encodeArray(const QList<T> &src);


    int offset() const;
    void setOffset(int offset);
    void truncateBufferToOffset();

private:
    bool enoughData(int requiredSize);
    template <typename T>
    T upperBound();

    QByteArray *m_data{nullptr};
    int m_offset{0};
};

template<typename T>
T QOpcUaBinaryDataEncoding::upperBound()
{
    // Use extra parentheses to prevent macro substitution for max() on windows
    return (std::numeric_limits<T>::max)();
}

template<typename T, QOpcUa::Types OVERLAY>
inline T QOpcUaBinaryDataEncoding::decode(bool &success)
{
    static_assert(OVERLAY == QOpcUa::Types::Undefined, "Ambiguous types are only permitted for template specializations");
    static_assert(std::is_arithmetic<T>::value == true, "Non-numeric types are only permitted for template specializations");

    if (!m_data) {
        success = false;
        return T(0);
    }

    if (enoughData(sizeof(T))) {
        T temp;
        memcpy(&temp, m_data->constData() + m_offset, sizeof(T));
        m_offset += sizeof(T);
        success = true;
        return qFromLittleEndian<T>(temp);
    } else {
        success = false;
        return T(0);
    }
}

template<>
inline bool QOpcUaBinaryDataEncoding::decode<bool>(bool &success)
{
    if (!m_data) {
        success = false;
        return success;
    }

    if (enoughData(sizeof(quint8))) {
        auto temp = *reinterpret_cast<const quint8 *>(m_data->constData() + m_offset);
        m_offset += sizeof(temp);
        success = true;
        return temp != 0;
    } else {
        success = false;
        return false;
    }
}

template<>
inline QString QOpcUaBinaryDataEncoding::decode<QString>(bool &success)
{
    if (!m_data) {
        success = false;
        return QString();
    }

    const auto length = decode<qint32>(success);

    if (length > 0 && !enoughData(length)) {
        success = false;
        return QString();
    }

    if (length > 0) {
        QString temp =  QString::fromUtf8(reinterpret_cast<const char *>(m_data->constData() + m_offset), length);
        m_offset += length;
        success = true;
        return temp;
    } else if (length == 0) { // Empty string
        success = true;
        return QString::fromUtf8("");
    } else if (length == -1) { // Null string
        success = true;
        return QString();
    }

    success = false;
    return QString();
}

template <>
inline QOpcUaQualifiedName QOpcUaBinaryDataEncoding::decode<QOpcUaQualifiedName>(bool &success)
{
    QOpcUaQualifiedName temp;
    temp.setNamespaceIndex(decode<quint16>(success));
    if (!success)
        return QOpcUaQualifiedName();

    temp.setName(decode<QString>(success));
    if (!success)
        return QOpcUaQualifiedName();

    return temp;
}

template <>
inline QOpcUaLocalizedText QOpcUaBinaryDataEncoding::decode<QOpcUaLocalizedText>(bool &success)
{
    QOpcUaLocalizedText temp;
    quint8 encodingMask = decode<quint8>(success);
    if (!success)
        return QOpcUaLocalizedText();

    if (encodingMask & 0x01) {
        temp.setLocale(decode<QString>(success));
        if (!success)
            return QOpcUaLocalizedText();
    }
    if (encodingMask & 0x02) {
        temp.setText(decode<QString>(success));
        if (!success)
            return QOpcUaLocalizedText();
    }
    return temp;
}

template <>
inline QOpcUaEUInformation QOpcUaBinaryDataEncoding::decode<QOpcUaEUInformation>(bool &success)
{
    QOpcUaEUInformation temp;

    temp.setNamespaceUri(decode<QString>(success));
    if (!success)
        return QOpcUaEUInformation();

    temp.setUnitId(decode<qint32>(success));
    if (!success)
        return QOpcUaEUInformation();

    temp.setDisplayName(decode<QOpcUaLocalizedText>(success));
    if (!success)
        return QOpcUaEUInformation();

    temp.setDescription(decode<QOpcUaLocalizedText>(success));
    if (!success)
        return QOpcUaEUInformation();

    return temp;
}

template <>
inline QOpcUaRange QOpcUaBinaryDataEncoding::decode<QOpcUaRange>(bool &success)
{
    QOpcUaRange temp;

    temp.setLow(decode<double>(success));
    if (!success)
        return QOpcUaRange();

    temp.setHigh(decode<double>(success));
    if (!success)
        return QOpcUaRange();

    return temp;
}

template <>
inline QOpcUaComplexNumber QOpcUaBinaryDataEncoding::decode<QOpcUaComplexNumber>(bool &success)
{
    QOpcUaComplexNumber temp;

    temp.setReal(decode<float>(success));
    if (!success)
        return QOpcUaComplexNumber();

    temp.setImaginary(decode<float>(success));
    if (!success)
        return QOpcUaComplexNumber();

    return temp;
}

template <>
inline QOpcUaDoubleComplexNumber QOpcUaBinaryDataEncoding::decode<QOpcUaDoubleComplexNumber>(bool &success)
{
    QOpcUaDoubleComplexNumber temp;

    temp.setReal(decode<double>(success));
    if (!success)
        return QOpcUaDoubleComplexNumber();

    temp.setImaginary(decode<double>(success));
    if (!success)
        return QOpcUaDoubleComplexNumber();

    return temp;
}

template <>
inline QOpcUaAxisInformation QOpcUaBinaryDataEncoding::decode<QOpcUaAxisInformation>(bool &success)
{
    QOpcUaAxisInformation temp;

    temp.setEngineeringUnits(decode<QOpcUaEUInformation>(success));
    if (!success)
        return QOpcUaAxisInformation();

    temp.setEURange(decode<QOpcUaRange>(success));
    if (!success)
        return QOpcUaAxisInformation();

    temp.setTitle(decode<QOpcUaLocalizedText>(success));
    if (!success)
        return QOpcUaAxisInformation();

    temp.setAxisScaleType(static_cast<QOpcUa::AxisScale>(decode<quint32>(success)));
    if (!success)
        return QOpcUaAxisInformation();

    temp.setAxisSteps(decodeArray<double>(success));
    if (!success)
        return QOpcUaAxisInformation();

    return temp;
}

template <>
inline QOpcUaXValue QOpcUaBinaryDataEncoding::decode<QOpcUaXValue>(bool &success)
{
    QOpcUaXValue temp;

    temp.setX(decode<double>(success));
    if (!success)
        return QOpcUaXValue();

    temp.setValue(decode<float>(success));
    if (!success)
        return QOpcUaXValue();

    return temp;
}

template <>
inline QUuid QOpcUaBinaryDataEncoding::decode<QUuid>(bool &success)
{
    if (!m_data) {
        success = false;
        return QUuid();
    }

    // An UUID is 16 bytes long
    const size_t uuidSize = 16;
    if (!enoughData(uuidSize)) {
        success = false;
        return QUuid();
    }

    const auto data1 = decode<quint32>(success);
    if (!success)
        return QUuid();

    const auto data2 = decode<quint16>(success);
    if (!success)
        return QUuid();

    const auto data3 = decode<quint16>(success);
    if (!success)
        return QUuid();

    const auto data4 = QByteArray::fromRawData(m_data->constData() + m_offset, 8);
    if (!success)
        return QUuid();

    m_offset += 8;

    const QUuid temp = QUuid(data1, data2, data3, data4[0], data4[1], data4[2],
                             data4[3], data4[4], data4[5], data4[6], data4[7]);

    success = true;
    return temp;
}

template <>
inline QByteArray QOpcUaBinaryDataEncoding::decode<QByteArray>(bool &success)
{
    if (!m_data) {
        success = false;
        return QByteArray();
    }

    qint32 size = decode<qint32>(success);
    if (!success)
        return QByteArray();

    if (size > 0 && enoughData(size)) {
        const QByteArray temp(m_data->constData() + m_offset, size);
        m_offset += size;
        return temp;
    } else if (size == 0) {
        return QByteArray("");
    } else if (size == -1) {
        return QByteArray();
    }

    success = false;
    return QByteArray();
}

template <>
inline QString QOpcUaBinaryDataEncoding::decode<QString, QOpcUa::Types::NodeId>(bool &success)
{
    quint8 identifierType = decode<quint8>(success);
    if (!success)
        return QString();

    identifierType &= ~(0x40 | 0x80); // Remove expanded node id flags

    quint16 namespaceIndex;

    if (identifierType == 0x00) {
        // encodingType 0x00 does not transfer the namespace index, it has to be zero
        // Part 6, Chapter 5.2.2.9, Section "Two Byte NodeId Binary DataEncoding"
        namespaceIndex = 0;
    } else if (identifierType == 0x01){
        // encodingType 0x01 transfers only one byte namespace index, has to be in range 0-255
        // Part 6, Chapter 5.2.2.9, Section "Four Byte NodeId Binary DataEncoding"
        namespaceIndex = decode<quint8>(success);
    } else {
        namespaceIndex = decode<quint16>(success);
    }

    if (!success)
        return QString();

    switch (identifierType) {
    case 0x00: {
        quint8 identifier = decode<quint8>(success);
        if (!success)
            return QString();
        return QStringLiteral("ns=%1;i=%2").arg(namespaceIndex).arg(identifier);
    }
    case 0x01: {
        quint16 identifier = decode<quint16>(success);
        if (!success)
            return QString();
        return QStringLiteral("ns=%1;i=%2").arg(namespaceIndex).arg(identifier);
    }
    case 0x02: {
        quint32 identifier = decode<quint32>(success);
        if (!success)
            return QString();
        return QStringLiteral("ns=%1;i=%2").arg(namespaceIndex).arg(identifier);
    }
    case 0x03: {
        QString identifier = decode<QString>(success);
        if (!success)
            return QString();
        return QStringLiteral("ns=%1;s=%2").arg(namespaceIndex).arg(identifier);
    }
    case 0x04: {
        QUuid identifier = decode<QUuid>(success);
        if (!success)
            return QString();
        return QStringLiteral("ns=%1;g=%2").arg(namespaceIndex).arg(QStringView(identifier.toString()).mid(1, 36)); // Remove enclosing {...}
    }
    case 0x05: {
        QByteArray identifier = decode<QByteArray>(success);
        if (!success)
            return QString();
        return QStringLiteral("ns=%1;b=%2").arg(namespaceIndex).arg(QString::fromLatin1(identifier.toBase64().constData()));
    }
    }

    success = false;
    return QString();
}

template <>
inline QOpcUaExpandedNodeId QOpcUaBinaryDataEncoding::decode<QOpcUaExpandedNodeId>(bool &success)
{
    if (!m_data) {
        success = false;
        return QOpcUaExpandedNodeId();
    }

    // Don't decode the first byte, it is required for decode<QString, QOpcUa::Types::NodeId>()
    if (!enoughData(sizeof(quint8))) {
        success = false;
        return QOpcUaExpandedNodeId();
    }
    bool hasNamespaceUri = *(reinterpret_cast<const quint8 *>(m_data->constData() + m_offset)) & 0x80;
    bool hasServerIndex = *(reinterpret_cast<const quint8 *>(m_data->constData() + m_offset)) & 0x40;

    QString nodeId = decode<QString, QOpcUa::Types::NodeId>(success);
    if (!success)
        return QOpcUaExpandedNodeId();

    QString namespaceUri;
    if (hasNamespaceUri) {
        namespaceUri = decode<QString>(success);
        if (!success)
            return QOpcUaExpandedNodeId();
    }

    quint32 serverIndex = 0;
    if (hasServerIndex) {
        serverIndex = decode<quint32>(success);
        if (!success)
            return QOpcUaExpandedNodeId();
    }

    return QOpcUaExpandedNodeId(namespaceUri, nodeId, serverIndex);
}

template <>
inline QDateTime QOpcUaBinaryDataEncoding::decode<QDateTime>(bool &success)
{
    qint64 timestamp = decode<qint64>(success);
    if (!success)
        return QDateTime();

    if (timestamp == 0 || timestamp == upperBound<qint64>())
        return QDateTime();

    // OPC-UA part 6, 5.2.2.5
    const QDateTime epochStart(QDate(1601, 1, 1), QTime(0, 0), QTimeZone::UTC);
    return epochStart.addMSecs(timestamp / 10000);
}

template <>
inline QOpcUa::UaStatusCode QOpcUaBinaryDataEncoding::decode<QOpcUa::UaStatusCode>(bool &success)
{
    quint32 value = decode<quint32>(success);
    if (!success)
        return QOpcUa::UaStatusCode(0);

    return QOpcUa::UaStatusCode(value);
}

template <>
inline QOpcUaExtensionObject QOpcUaBinaryDataEncoding::decode<QOpcUaExtensionObject>(bool &success)
{
    QOpcUaExtensionObject temp;

    QString typeId = decode<QString, QOpcUa::Types::NodeId>(success);
    if (!success)
        return QOpcUaExtensionObject();

    temp.setEncodingTypeId(typeId);
    quint8 encoding = decode<quint8>(success);
    if (!success || encoding > 2) {
        success = false;
        return QOpcUaExtensionObject();
    }
    temp.setEncoding(QOpcUaExtensionObject::Encoding(encoding));
    if (encoding == 0)
        return temp;

    QByteArray body = decode<QByteArray>(success);
    if (!success)
        return QOpcUaExtensionObject();

    temp.setEncodedBody(body);
    return temp;
}

template <>
inline QOpcUaArgument QOpcUaBinaryDataEncoding::decode<QOpcUaArgument>(bool &success)
{
    QOpcUaArgument temp;

    temp.setName(decode<QString>(success));
    if (!success)
        return QOpcUaArgument();

    temp.setDataTypeId(decode<QString, QOpcUa::Types::NodeId>(success));
    if (!success)
        return QOpcUaArgument();

    temp.setValueRank(decode<qint32>(success));
    if (!success)
        return QOpcUaArgument();

    temp.setArrayDimensions(decodeArray<quint32>(success));
    if (!success)
        return QOpcUaArgument();

    temp.setDescription(decode<QOpcUaLocalizedText>(success));
    if (!success)
        return QOpcUaArgument();

    return temp;
}

template<typename T, QOpcUa::Types OVERLAY>
inline bool QOpcUaBinaryDataEncoding::encode(const T &src)
{
    static_assert(OVERLAY == QOpcUa::Types::Undefined, "Ambiguous types are only permitted for template specializations");
    static_assert(std::is_arithmetic<T>::value == true, "Non-numeric types are only permitted for template specializations");

    if (!m_data)
        return false;

    T temp = qToLittleEndian<T>(src);
    m_data->append(reinterpret_cast<const char *>(&temp), sizeof(T));
    return true;
}

template<>
inline bool QOpcUaBinaryDataEncoding::encode<bool>(const bool &src)
{
    if (!m_data)
        return false;

    const quint8 value = src ? 1 : 0;
    m_data->append(reinterpret_cast<const char *>(&value), sizeof(value));
    return true;
}

template<>
inline bool QOpcUaBinaryDataEncoding::encode<QString>(const QString &src)
{
    if (!m_data)
        return false;

    if (src.size() > upperBound<qint32>())
        return false;

    QByteArray arr = src.toUtf8();
    if (!encode<qint32>(arr.isNull() ? -1 : int(arr.size())))
        return false;
    m_data->append(arr);
    return true;
}

template<>
inline bool QOpcUaBinaryDataEncoding::encode<QOpcUaQualifiedName>(const QOpcUaQualifiedName &src)
{
    if (!encode<quint16>(src.namespaceIndex()))
        return false;
    if (!encode<QString>(src.name()))
        return false;
    return true;
}

template<>
inline bool QOpcUaBinaryDataEncoding::encode<QOpcUaLocalizedText>(const QOpcUaLocalizedText &src)
{
    quint8 mask = 0;
    if (src.locale().size() != 0)
        mask |= 0x01;
    if (src.text().size() != 0)
        mask |= 0x02;
    if (!encode<quint8>(mask))
        return false;
    if (src.locale().size())
        if (!encode<QString>(src.locale()))
            return false;
    if (src.text().size())
        if (!encode<QString>(src.text()))
            return false;
    return true;
}

template <>
inline bool QOpcUaBinaryDataEncoding::encode<QOpcUaRange>(const QOpcUaRange &src)
{
    if (!encode<double>(src.low()))
        return false;
    if (!encode<double>(src.high()))
        return false;
    return true;
}

template <>
inline bool QOpcUaBinaryDataEncoding::encode<QOpcUaEUInformation>(const QOpcUaEUInformation &src)
{
    if (!encode<QString>(src.namespaceUri()))
        return false;
    if (!encode<qint32>(src.unitId()))
        return false;
    if (!encode<QOpcUaLocalizedText>(src.displayName()))
        return false;
    if (!encode<QOpcUaLocalizedText>(src.description()))
        return false;
    return true;
}

template <>
inline bool QOpcUaBinaryDataEncoding::encode<QOpcUaComplexNumber>(const QOpcUaComplexNumber &src)
{
    if (!encode<float>(src.real()))
        return false;
    if (!encode<float>(src.imaginary()))
        return false;
    return true;
}

template <>
inline bool QOpcUaBinaryDataEncoding::encode<QOpcUaDoubleComplexNumber>(const QOpcUaDoubleComplexNumber &src)
{
    if (!encode<double>(src.real()))
        return false;
    if (!encode<double>(src.imaginary()))
        return false;
    return true;
}

template <>
inline bool QOpcUaBinaryDataEncoding::encode<QOpcUaAxisInformation>(const QOpcUaAxisInformation &src)
{
    if (!encode<QOpcUaEUInformation>(src.engineeringUnits()))
        return false;
    if (!encode<QOpcUaRange>(src.eURange()))
        return false;
    if (!encode<QOpcUaLocalizedText>(src.title()))
        return false;
    if (!encode<quint32>(static_cast<quint32>(src.axisScaleType())))
        return false;
    if (!encodeArray<double>(src.axisSteps()))
        return false;
    return true;
}

template <>
inline bool QOpcUaBinaryDataEncoding::encode<QOpcUaXValue>(const QOpcUaXValue &src)
{
    if (!encode<double>(src.x()))
        return false;
    if (!encode<float>(src.value()))
        return false;
    return true;
}

template <>
inline bool QOpcUaBinaryDataEncoding::encode<QUuid>(const QUuid &src)
{
    if (!encode<quint32>(src.data1))
        return false;
    if (!encode<quint16>(src.data2))
        return false;
    if (!encode<quint16>(src.data3))
        return false;

    auto data = QByteArray::fromRawData(reinterpret_cast<const char *>(src.data4), sizeof(src.data4));
    m_data->append(data);

    return true;
}

template <>
inline bool QOpcUaBinaryDataEncoding::encode<QByteArray>(const QByteArray &src)
{
    if (!m_data)
        return false;

    if (src.size() > upperBound<qint32>())
        return false;

    if (!encode<qint32>(src.isNull() ? -1 : int(src.size())))
        return false;
    if (src.size() > 1)
        m_data->append(src);
    return true;
}

template <>
inline bool QOpcUaBinaryDataEncoding::encode<QString, QOpcUa::Types::NodeId>(const QString &src)
{
    if (!m_data)
        return false;

    quint16 index;
    QString identifier;
    char type;
    if (!QOpcUa::nodeIdStringSplit(src, &index, &identifier, &type))
        return false;

    qint32 identifierType;
    switch (type) {
    case 'i':
        identifierType = 0;
        break;
    case 's':
        identifierType = 1;
        break;
    case 'g':
        identifierType = 2;
        break;
    case 'b':
        identifierType = 3;
        break;
    default:
        return false;
    }

    QByteArray encodedIdentifier;
    QOpcUaBinaryDataEncoding encoder(&encodedIdentifier);
    quint8 encodingType = 0;

    switch (identifierType) {
    case 0: {
        bool isNumber;
        uint integerIdentifier = identifier.toUInt(&isNumber);
        if (!isNumber || integerIdentifier > upperBound<quint32>())
            return false;

        if (integerIdentifier <= 255 && index == 0) {
            // encodingType 0x00 does not transfer the namespace index, it has to be zero
            // Part 6, Chapter 5.2.2.9, Section "Two Byte NodeId Binary DataEncoding"
            if (!encoder.encode<quint8>(integerIdentifier))
                return false;
            encodingType = 0x00; // 8 bit numeric
            break;
        } else if (integerIdentifier <= 65535 && index <= 255) {
            // encodingType 0x01 transfers only one byte namespace index, has to be in range 0-255
            // Part 6, Chapter 5.2.2.9, Section "Four Byte NodeId Binary DataEncoding"
            if (!encoder.encode<quint16>(integerIdentifier))
                return false;
            encodingType = 0x01; // 16 bit numeric
            break;
        } else {
            if (!encoder.encode<quint32>(integerIdentifier))
                return false;
            encodingType = 0x02; // 32 bit numeric
            break;
        }
    }
    case 1: {
        if (identifier.isEmpty())
            return false;
        if (!encoder.encode<QString>(identifier))
            return false;
        encodingType = 0x03; // String
        break;
    }
    case 2: {
        QUuid uuid(identifier);
        if (uuid.isNull())
            return false;
        if (!encoder.encode<QUuid>(uuid))
            return false;
        encodingType = 0x04; // GUID
        break;
    }
    case 3: {
        const QByteArray temp = QByteArray::fromBase64(identifier.toLatin1());
        if (temp.isEmpty())
            return false;
        if (!encoder.encode<QByteArray>(temp))
            return false;
        encodingType = 0x05; // ByteString
        break;
    }
    default:
        return false;
    }

    if (!encode<quint8>(encodingType))
        return false;

    if (encodingType == 0x00) {
        // encodingType == 0x00 skips namespace completely, defaults to zero
        // Part 6, Chapter 5.2.2.9, Section "Two Byte NodeId Binary DataEncoding"
    } else if (encodingType == 0x01) {
        if (!encode<quint8>(index))
            return false;
    } else {
        if (!encode<quint16>(index))
            return false;
    }

    m_data->append(encodedIdentifier);
    return true;
}

template <>
inline bool QOpcUaBinaryDataEncoding::encode<QOpcUaExpandedNodeId>(const QOpcUaExpandedNodeId &src)
{
    if (!m_data)
        return false;

    QByteArray temp;
    QOpcUaBinaryDataEncoding encoder(&temp);
    if (!encoder.encode<QString, QOpcUa::Types::NodeId>(src.nodeId()))
        return false;

    quint8 mask = temp.at(0);

    if (!src.namespaceUri().isEmpty()) {
        mask |= 0x80;
        if (!encoder.encode<QString>(src.namespaceUri()))
            return false;
    }

    if (src.serverIndex() != 0) {
        mask |= 0x40;
        if (!encoder.encode<quint32>(src.serverIndex()))
            return false;
    }

    temp[0] = mask;

    m_data->append(temp);
    return true;
}

template <>
inline bool QOpcUaBinaryDataEncoding::encode<QDateTime>(const QDateTime &src)
{
    // OPC-UA part 6, 5.2.2.5
    if (src >= QDateTime(QDate(9999, 12, 31), QTime(11, 59, 59))) {
        if (!encode<qint64>(upperBound<qint64>()))
            return false;
        return true;
    }

    const QDateTime uaEpochStart(QDate(1601, 1, 1), QTime(0, 0), QTimeZone::UTC);

    if (src <= uaEpochStart) {
        if (!encode<qint64>(0))
            return false;
        return true;
    }

    qint64 timestamp = 10000 * (src.toMSecsSinceEpoch() - uaEpochStart.toMSecsSinceEpoch());
    if (!encode<qint64>(timestamp))
        return false;
    return true;
}

template <>
inline bool QOpcUaBinaryDataEncoding::encode<QOpcUa::UaStatusCode>(const QOpcUa::UaStatusCode &src)
{
    if (!encode<quint32>(src))
        return false;
    return true;
}

template <>
inline bool QOpcUaBinaryDataEncoding::encode<QOpcUaExtensionObject>(const QOpcUaExtensionObject &src)
{
    if (!encode<QString, QOpcUa::Types::NodeId>(src.encodingTypeId()))
        return false;

    if (!encode<quint8>(quint8(src.encoding())))
        return false;
    if (src.encoding() != QOpcUaExtensionObject::Encoding::NoBody)
        if (!encode<QByteArray>(src.encodedBody()))
            return false;

    return true;
}

template <>
inline bool QOpcUaBinaryDataEncoding::encode<QOpcUaArgument>(const QOpcUaArgument &src)
{
    if (!m_data)
        return false;

    QByteArray temp;
    QOpcUaBinaryDataEncoding encoder(&temp);
    if (!encoder.encode<QString>(src.name()))
        return false;
    if (!encoder.encode<QString, QOpcUa::Types::NodeId>(src.dataTypeId()))
        return false;

    if (!encoder.encode<qint32>(src.valueRank()))
        return false;
    if (!encoder.encodeArray<quint32>(src.arrayDimensions()))
        return false;
    if (!encoder.encode<QOpcUaLocalizedText>(src.description()))
        return false;
    m_data->append(temp);

    return true;
}

template<typename T, QOpcUa::Types OVERLAY>
inline QList<T> QOpcUaBinaryDataEncoding::decodeArray(bool &success)
{
    QList<T> temp;

    qint32 size = decode<qint32>(success);
    if (!success)
        return temp;

    for (int i = 0; i < size; ++i) {
        temp.push_back(decode<T, OVERLAY>(success));
        if (!success)
            return QList<T>();
    }

    return temp;
}

template<typename T, QOpcUa::Types OVERLAY>
inline bool QOpcUaBinaryDataEncoding::encodeArray(const QList<T> &src)
{
    if (src.size() > upperBound<qint32>())
        return false;

    if (!encode<qint32>(int(src.size())))
        return false;
    for (const auto &element : src) {
        if (!encode<T, OVERLAY>(element))
            return false;
    }
    return true;
}

template <>
inline QOpcUaApplicationRecordDataType QOpcUaBinaryDataEncoding::decode<QOpcUaApplicationRecordDataType>(bool &success)
{
    QOpcUaApplicationRecordDataType temp;

    temp.setApplicationId(decode<QString, QOpcUa::Types::NodeId>(success));
    if (!success)
        return QOpcUaApplicationRecordDataType();

    temp.setApplicationUri(decode<QString>(success));
    if (!success)
        return QOpcUaApplicationRecordDataType();

    temp.setApplicationType(static_cast<QOpcUaApplicationDescription::ApplicationType>(decode<uint32_t>(success)));
    if (!success)
        return QOpcUaApplicationRecordDataType();

    temp.setApplicationNames(decodeArray<QOpcUaLocalizedText>(success));
    if (!success)
        return QOpcUaApplicationRecordDataType();

    temp.setProductUri(decode<QString>(success));
    if (!success)
        return QOpcUaApplicationRecordDataType();

    temp.setDiscoveryUrls(decodeArray<QString>(success));
    if (!success)
        return QOpcUaApplicationRecordDataType();

    temp.setServerCapabilityIdentifiers(decodeArray<QString>(success));
    if (!success)
        return QOpcUaApplicationRecordDataType();

    return temp;
}

template <>
inline bool QOpcUaBinaryDataEncoding::encode<QOpcUaApplicationRecordDataType>(const QOpcUaApplicationRecordDataType &src)
{
    if (!encode<QString, QOpcUa::NodeId>(src.applicationId()))
        return false;
    if (!encode<QString>(src.applicationUri()))
        return false;
    if (!encode<uint32_t>(src.applicationType()))
        return false;
    if (!encodeArray<QOpcUaLocalizedText>(src.applicationNames()))
        return false;
    if (!encode<QString>(src.productUri()))
        return false;
    if (!encodeArray<QString>(src.discoveryUrls()))
        return false;
    if (!encodeArray<QString>(src.serverCapabilityIdentifiers()))
        return false;
    return true;
}

QT_END_NAMESPACE

#endif // QOPCUABINARYDATAENCODING_H
