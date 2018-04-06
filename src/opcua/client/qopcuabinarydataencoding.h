/****************************************************************************
**
** Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QOPCUABINARYDATAENCODING_H
#define QOPCUABINARYDATAENCODING_H

#include <QtOpcUa/qopcuatype.h>

#include <QtCore/qdatetime.h>
#include <QtCore/qmetatype.h>
#include <QtCore/quuid.h>
#include <QtCore/qendian.h>

#include <limits>

QT_BEGIN_NAMESPACE

// This class implements a subset of the OPC UA Binary DataEncoding defined in OPC-UA part 6, 5.2.
class Q_OPCUA_EXPORT QOpcUaBinaryDataEncoding
{
public:

    QOpcUaBinaryDataEncoding(QByteArray *buffer);
    QOpcUaBinaryDataEncoding(QOpcUa::QExtensionObject &object);

    // The Ids in this enum are the numeric Ids of the _Encoding_DefaultBinary nodes for the respective types
    // as listed in https://opcfoundation.org/UA/schemas/1.03/NodeIds.csv
    enum class TypeEncodingId {
        Range = 886,
        EUInformation = 889,
        ComplexNumber = 12181,
        DoubleComplexNumber = 12182,
        AxisInformation = 12089,
        XV = 12090
    };

    template <typename T, QOpcUa::Types OVERLAY = QOpcUa::Types::Undefined>
    T decode(bool &success);
    template <typename T, QOpcUa::Types OVERLAY = QOpcUa::Types::Undefined>
    QVector<T> decodeArray(bool &success);

    template <typename T, QOpcUa::Types OVERLAY = QOpcUa::Types::Undefined>
    bool encode(const T &src);
    template <typename T, QOpcUa::Types OVERLAY = QOpcUa::Types::Undefined>
    bool encodeArray(const QVector<T> &src);


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
        T temp = *reinterpret_cast<const T *>(m_data->constData() + m_offset);
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

    if (length > 0 && !enoughData(static_cast<size_t>(length))) {
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
inline QOpcUa::QQualifiedName QOpcUaBinaryDataEncoding::decode<QOpcUa::QQualifiedName>(bool &success)
{
    QOpcUa::QQualifiedName temp;
    temp.setNamespaceIndex(decode<quint16>(success));
    if (!success)
        return QOpcUa::QQualifiedName();

    temp.setName(decode<QString>(success));
    if (!success)
        return QOpcUa::QQualifiedName();

    return temp;
}

template <>
inline QOpcUa::QLocalizedText QOpcUaBinaryDataEncoding::decode<QOpcUa::QLocalizedText>(bool &success)
{
    QOpcUa::QLocalizedText temp;
    quint8 encodingMask = decode<quint8>(success);
    if (!success)
        return QOpcUa::QLocalizedText();

    if (encodingMask & 0x01) {
        temp.setLocale(decode<QString>(success));
        if (!success)
            return QOpcUa::QLocalizedText();
    }
    if (encodingMask & 0x02) {
        temp.setText(decode<QString>(success));
        if (!success)
            return QOpcUa::QLocalizedText();
    }
    return temp;
}

template <>
inline QOpcUa::QEUInformation QOpcUaBinaryDataEncoding::decode<QOpcUa::QEUInformation>(bool &success)
{
    QOpcUa::QEUInformation temp;

    temp.setNamespaceUri(decode<QString>(success));
    if (!success)
        return QOpcUa::QEUInformation();

    temp.setUnitId(decode<qint32>(success));
    if (!success)
        return QOpcUa::QEUInformation();

    temp.setDisplayName(decode<QOpcUa::QLocalizedText>(success));
    if (!success)
        return QOpcUa::QEUInformation();

    temp.setDescription(decode<QOpcUa::QLocalizedText>(success));
    if (!success)
        return QOpcUa::QEUInformation();

    return temp;
}

template <>
inline QOpcUa::QRange QOpcUaBinaryDataEncoding::decode<QOpcUa::QRange>(bool &success)
{
    QOpcUa::QRange temp;

    temp.setLow(decode<double>(success));
    if (!success)
        return QOpcUa::QRange();

    temp.setHigh(decode<double>(success));
    if (!success)
        return QOpcUa::QRange();

    return temp;
}

template <>
inline QOpcUa::QComplexNumber QOpcUaBinaryDataEncoding::decode<QOpcUa::QComplexNumber>(bool &success)
{
    QOpcUa::QComplexNumber temp;

    temp.setReal(decode<float>(success));
    if (!success)
        return QOpcUa::QComplexNumber();

    temp.setImaginary(decode<float>(success));
    if (!success)
        return QOpcUa::QComplexNumber();

    return temp;
}

template <>
inline QOpcUa::QDoubleComplexNumber QOpcUaBinaryDataEncoding::decode<QOpcUa::QDoubleComplexNumber>(bool &success)
{
    QOpcUa::QDoubleComplexNumber temp;

    temp.setReal(decode<double>(success));
    if (!success)
        return QOpcUa::QDoubleComplexNumber();

    temp.setImaginary(decode<double>(success));
    if (!success)
        return QOpcUa::QDoubleComplexNumber();

    return temp;
}

template <>
inline QOpcUa::QAxisInformation QOpcUaBinaryDataEncoding::decode<QOpcUa::QAxisInformation>(bool &success)
{
    QOpcUa::QAxisInformation temp;

    temp.setEngineeringUnits(decode<QOpcUa::QEUInformation>(success));
    if (!success)
        return QOpcUa::QAxisInformation();

    temp.setEURange(decode<QOpcUa::QRange>(success));
    if (!success)
        return QOpcUa::QAxisInformation();

    temp.setTitle(decode<QOpcUa::QLocalizedText>(success));
    if (!success)
        return QOpcUa::QAxisInformation();

    temp.setAxisScaleType(static_cast<QOpcUa::AxisScale>(decode<quint32>(success)));
    if (!success)
        return QOpcUa::QAxisInformation();

    temp.setAxisSteps(decodeArray<double>(success));
    if (!success)
        return QOpcUa::QAxisInformation();

    return temp;
}

template <>
inline QOpcUa::QXValue QOpcUaBinaryDataEncoding::decode<QOpcUa::QXValue>(bool &success)
{
    QOpcUa::QXValue temp;

    temp.setX(decode<double>(success));
    if (!success)
        return QOpcUa::QXValue();

    temp.setValue(decode<float>(success));
    if (!success)
        return QOpcUa::QXValue();

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

    const QUuid temp = QUuid::fromRfc4122(QByteArray::fromRawData(m_data->constData() + m_offset, uuidSize));
    m_offset += uuidSize;
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

    quint16 namespaceIndex = decode<quint16>(success);
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
        return QStringLiteral("ns=%1;g=%2").arg(namespaceIndex).arg(identifier.toString().midRef(1, 36)); // Remove enclosing {...}
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
inline QOpcUa::QExpandedNodeId QOpcUaBinaryDataEncoding::decode<QOpcUa::QExpandedNodeId>(bool &success)
{
    if (!m_data) {
        success = false;
        return QOpcUa::QExpandedNodeId();
    }

    // Don't decode the first byte, it is required for decode<QString, QOpcUa::Types::NodeId>()
    if (!enoughData(sizeof(quint8))) {
        success = false;
        return QOpcUa::QExpandedNodeId();
    }
    bool hasNamespaceUri = *(reinterpret_cast<const quint8 *>(m_data->constData() + m_offset)) & 0x80;
    bool hasServerIndex = *(reinterpret_cast<const quint8 *>(m_data->constData() + m_offset)) & 0x40;

    QString nodeId = decode<QString, QOpcUa::Types::NodeId>(success);
    if (!success)
        return QOpcUa::QExpandedNodeId();

    QString namespaceUri;
    if (hasNamespaceUri) {
        namespaceUri = decode<QString>(success);
        if (!success)
            return QOpcUa::QExpandedNodeId();
    }

    quint32 serverIndex = 0;
    if (hasServerIndex) {
        serverIndex = decode<quint32>(success);
        if (!success)
            return QOpcUa::QExpandedNodeId();
    }

    return QOpcUa::QExpandedNodeId(namespaceUri, nodeId, serverIndex);
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
    const QDateTime epochStart(QDate(1601, 1, 1), QTime(0, 0), Qt::UTC);
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
inline QOpcUa::QExtensionObject QOpcUaBinaryDataEncoding::decode<QOpcUa::QExtensionObject>(bool &success)
{
    QOpcUa::QExtensionObject temp;

    QString typeId = decode<QString, QOpcUa::Types::NodeId>(success);
    if (!success)
        return QOpcUa::QExtensionObject();

    temp.setEncodingTypeId(typeId);
    quint8 encoding = decode<quint8>(success);
    if (!success || encoding > 2) {
        success = false;
        return QOpcUa::QExtensionObject();
    }
    temp.setEncoding(QOpcUa::QExtensionObject::Encoding(encoding));
    if (encoding == 0)
        return temp;

    QByteArray body = decode<QByteArray>(success);
    if (!success)
        return QOpcUa::QExtensionObject();

    temp.setEncodedBody(body);
    return temp;
}

template <>
inline QOpcUa::QArgument QOpcUaBinaryDataEncoding::decode<QOpcUa::QArgument>(bool &success)
{
    QOpcUa::QArgument temp;

    temp.setName(decode<QString>(success));
    if (!success)
        return QOpcUa::QArgument();

    temp.setDataTypeId(decode<QString, QOpcUa::Types::NodeId>(success));
    if (!success)
        return QOpcUa::QArgument();

    temp.setValueRank(decode<qint32>(success));
    if (!success)
        return QOpcUa::QArgument();

    temp.setArrayDimensions(decodeArray<quint32>(success));
    if (!success)
        return QOpcUa::QArgument();

    temp.setDescription(decode<QOpcUa::QLocalizedText>(success));
    if (!success)
        return QOpcUa::QArgument();

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
    if (!encode<qint32>(arr.isNull() ? -1 : arr.length()))
        return false;
    m_data->append(arr);
    return true;
}

template<>
inline bool QOpcUaBinaryDataEncoding::encode<QOpcUa::QQualifiedName>(const QOpcUa::QQualifiedName &src)
{
    if (!encode<quint16>(src.namespaceIndex()))
        return false;
    if (!encode<QString>(src.name()))
        return false;
    return true;
}

template<>
inline bool QOpcUaBinaryDataEncoding::encode<QOpcUa::QLocalizedText>(const QOpcUa::QLocalizedText &src)
{
    quint8 mask = 0;
    if (src.locale().length() != 0)
        mask |= 0x01;
    if (src.text().length() != 0)
        mask |= 0x02;
    if (!encode<quint8>(mask))
        return false;
    if (src.locale().length())
        if (!encode<QString>(src.locale()))
            return false;
    if (src.text().length())
        if (!encode<QString>(src.text()))
            return false;
    return true;
}

template <>
inline bool QOpcUaBinaryDataEncoding::encode<QOpcUa::QRange>(const QOpcUa::QRange &src)
{
    if (!encode<double>(src.low()))
        return false;
    if (!encode<double>(src.high()))
        return false;
    return true;
}

template <>
inline bool QOpcUaBinaryDataEncoding::encode<QOpcUa::QEUInformation>(const QOpcUa::QEUInformation &src)
{
    if (!encode<QString>(src.namespaceUri()))
        return false;
    if (!encode<qint32>(src.unitId()))
        return false;
    if (!encode<QOpcUa::QLocalizedText>(src.displayName()))
        return false;
    if (!encode<QOpcUa::QLocalizedText>(src.description()))
        return false;
    return true;
}

template <>
inline bool QOpcUaBinaryDataEncoding::encode<QOpcUa::QComplexNumber>(const QOpcUa::QComplexNumber &src)
{
    if (!encode<float>(src.real()))
        return false;
    if (!encode<float>(src.imaginary()))
        return false;
    return true;
}

template <>
inline bool QOpcUaBinaryDataEncoding::encode<QOpcUa::QDoubleComplexNumber>(const QOpcUa::QDoubleComplexNumber &src)
{
    if (!encode<double>(src.real()))
        return false;
    if (!encode<double>(src.imaginary()))
        return false;
    return true;
}

template <>
inline bool QOpcUaBinaryDataEncoding::encode<QOpcUa::QAxisInformation>(const QOpcUa::QAxisInformation &src)
{
    if (!encode<QOpcUa::QEUInformation>(src.engineeringUnits()))
        return false;
    if (!encode<QOpcUa::QRange>(src.eURange()))
        return false;
    if (!encode<QOpcUa::QLocalizedText>(src.title()))
        return false;
    if (!encode<quint32>(static_cast<quint32>(src.axisScaleType())))
        return false;
    if (!encodeArray<double>(src.axisSteps()))
        return false;
    return true;
}

template <>
inline bool QOpcUaBinaryDataEncoding::encode<QOpcUa::QXValue>(const QOpcUa::QXValue &src)
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
    if (!m_data)
        return false;

    m_data->append(src.toRfc4122());
    return true;
}

template <>
inline bool QOpcUaBinaryDataEncoding::encode<QByteArray>(const QByteArray &src)
{
    if (!m_data)
        return false;

    if (src.size() > upperBound<qint32>())
        return false;

    if (!encode<qint32>(src.isNull() ? -1 : src.size()))
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

        if (integerIdentifier <= 255) {
            if (!encoder.encode<quint8>(integerIdentifier))
                return false;
            encodingType = 0x00; // 8 bit numeric
            break;
        } else if (integerIdentifier <= 65535) {
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
    if (!encode<quint16>(index))
        return false;
    m_data->append(encodedIdentifier);
    return true;
}

template <>
inline bool QOpcUaBinaryDataEncoding::encode<QOpcUa::QExpandedNodeId>(const QOpcUa::QExpandedNodeId &src)
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

    const QDateTime uaEpochStart(QDate(1601, 1, 1), QTime(0, 0), Qt::UTC);

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
inline bool QOpcUaBinaryDataEncoding::encode<QOpcUa::QExtensionObject>(const QOpcUa::QExtensionObject &src)
{
    if (!encode<QString, QOpcUa::Types::NodeId>(src.encodingTypeId()))
        return false;

    if (!encode<quint8>(quint8(src.encoding())))
        return false;
    if (src.encoding() != QOpcUa::QExtensionObject::Encoding::NoBody)
        if (!encode<QByteArray>(src.encodedBody()))
            return false;

    return true;
}

template <>
inline bool QOpcUaBinaryDataEncoding::encode<QOpcUa::QArgument>(const QOpcUa::QArgument &src)
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
    if (!encoder.encode<QOpcUa::QLocalizedText>(src.description()))
        return false;
    m_data->append(temp);

    return true;
}

template<typename T, QOpcUa::Types OVERLAY>
inline QVector<T> QOpcUaBinaryDataEncoding::decodeArray(bool &success)
{
    QVector<T> temp;

    qint32 size = decode<qint32>(success);
    if (!success)
        return temp;

    for (int i = 0; i < size; ++i) {
        temp.push_back(decode<T, OVERLAY>(success));
        if (!success)
            return QVector<T>();
    }

    return temp;
}

template<typename T, QOpcUa::Types OVERLAY>
inline bool QOpcUaBinaryDataEncoding::encodeArray(const QVector<T> &src)
{
    if (src.size() > upperBound<qint32>())
        return false;

    if (!encode<qint32>(src.size()))
        return false;
    for (const auto &element : src) {
        if (!encode<T, OVERLAY>(element))
            return false;
    }
    return true;
}

QT_END_NAMESPACE

#endif // QOPCUABINARYDATAENCODING_H
