// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuabinarydataencoding.h"

#include <QtOpcUa/qopcuaapplicationrecorddatatype.h>
#include <QtOpcUa/qopcuaargument.h>
#include <QtOpcUa/qopcuaaxisinformation.h>
#include <QtOpcUa/qopcuacomplexnumber.h>
#include <QtOpcUa/qopcuadatavalue.h>
#include <QtOpcUa/qopcuadiagnosticinfo.h>
#include <QtOpcUa/qopcuadoublecomplexnumber.h>
#include <QtOpcUa/qopcuaenumdefinition.h>
#include <QtOpcUa/qopcuaenumfield.h>
#include <QtOpcUa/qopcuaeuinformation.h>
#include <QtOpcUa/qopcuaexpandednodeid.h>
#include <QtOpcUa/qopcuaextensionobject.h>
#include <QtOpcUa/qopcualocalizedtext.h>
#include <QtOpcUa/qopcuaqualifiedname.h>
#include <QtOpcUa/qopcuarange.h>
#include <QtOpcUa/qopcuastructuredefinition.h>
#include <QtOpcUa/qopcuastructurefield.h>
#include <QtOpcUa/qopcuaxvalue.h>

#include <QtCore/qbytearray.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qtimezone.h>
#include <QtCore/quuid.h>

QT_BEGIN_NAMESPACE

using namespace Qt::Literals::StringLiterals;

/*!
    \class QOpcUaBinaryDataEncoding
    \inmodule QtOpcUa
    \brief QOpcUaBinaryDataEncoding is a partial implementation of the OPC UA binary data encoding described in OPC UA part 6.

    It offers template functions for encoding and decoding data for reading and writing extension objects.

    The following types are supported:

    \table
        \header
            \li Qt type
            \li OPC UA type
        \row
            \li quint8
            \li uint8
        \row
            \li qint8
            \li int8
        \row
            \li quint16
            \li uint16
        \row
            \li qint16
            \li int16
        \row
            \li quint32
            \li uint32
        \row
            \li qint32
            \li int32
        \row
            \li quint64
            \li uint64
        \row
            \li qint64
            \li int64
        \row
            \li float
            \li float
        \row
            \li double
            \li double
        \row
            \li QString
            \li String
        \row
            \li QOpcUaQualifiedName
            \li QualifiedName
        \row
            \li QOpcUaLocalizedText
            \li LocalizedText
        \row
            \li QOpcUaEUInformation
            \li EUInformation
        \row
            \li QOpcUaRange
            \li Range
        \row
            \li QOpcUaComplexNumber
            \li ComplexNumber
        \row
            \li QOpcUaDoubleComplexNumber
            \li DoubleComplexNumber
        \row
            \li QOpcUaAxisInformation
            \li AxisInformation
        \row
            \li QOpcUaXValue
            \li XV
        \row
            \li QUuid
            \li GUID
        \row
            \li QString node id
            \li NodeId
        \row
            \li QByteArray
            \li ByteString
        \row
            \li QDateTime
            \li DateTime
        \row
            \li QOpcUa::UaStatusCode
            \li StatusCode
        \row
            \li QOpcUaExpandedNodeId
            \li ExpandedNodeId
        \row
            \li QOpcUaExtensionObject
            \li ExtensionObject
        \row
            \li QOpcUaArgument
            \li Argument
        \row
            \li QOpcUaDiagnosticInfo (since Qt 6.7)
            \li DiagnosticInfo
        \row
            \li QOpcUaApplicationRecordDataType
            \li ApplicationRecordDataType
        \row
            \li QOpcUaStructureDefinition (since Qt 6.7)
            \li StructureDefintion
        \row
            \li QOpcUaStructureField (since Qt 6.7)
            \li StructureField
        \row
            \li QOpcUaEnumDefinition (since Qt 6.7)
            \li EnumDefintion
        \row
            \li QOpcUaEnumField (since Qt 6.7)
            \li EnumField
        \row
            \li QOpcUaVariant (since Qt 6.7)
            \li Variant
        \row
            \li QOpcUaDataValue (since Qt 6.7)
            \li DataValue
    \endtable
*/

/*!
    \fn template<typename T, QOpcUa::Types OVERLAY> T QOpcUaBinaryDataEncoding::decode(bool &success)

    Decodes a scalar value of type T from the data buffer.
    \a success is set to \c true if the decoding was successful, \c false if not.

    The decoded value is returned. If \a success is false, the returned value is invalid.

    \sa decodeArray()
*/

/*!
    \fn template<typename T, QOpcUa::Types OVERLAY> bool QOpcUaBinaryDataEncoding::encode(const T &src)

    Encodes \a src of type T and appends the encoded value to the data buffer.
    Returns \c true if the value has been successfully encoded.

    \sa encodeArray()
*/

/*!
    \fn template<typename T, QOpcUa::Types OVERLAY> QList<T> QOpcUaBinaryDataEncoding::decodeArray(bool &success)

    Decodes an array of type T from the data buffer.
    \a success is set to \c true if the decoding was successful, \c false if not.

    The decoded value is returned. If \a success is false, the returned value is invalid.

    \sa decode()
*/

/*!
    \fn template<typename T, QOpcUa::Types OVERLAY> bool QOpcUaBinaryDataEncoding::encodeArray(const QList<T> &src)

    Encodes all elements of type T in \a src and appends the encoded values to the data buffer.

    Returns \c true if the value has been successfully encoded.

    \sa encode()
*/

/*!
    Constructs a binary data encoding object for the data buffer \a buffer.
    \a buffer must not be deleted as long as this binary data encoding object is used.
*/
QOpcUaBinaryDataEncoding::QOpcUaBinaryDataEncoding(QByteArray *buffer)
    : m_data(buffer)
{
}

/*!
    Constructs a binary data encoding object using the encoded body of \a object as data buffer.

    \a object must not be deleted as long as this binary data encoding object is used.
*/
QOpcUaBinaryDataEncoding::QOpcUaBinaryDataEncoding(QOpcUaExtensionObject &object)
    : m_data(&object.encodedBodyRef())
{
}

bool QOpcUaBinaryDataEncoding::enoughData(int requiredSize)
{
    if (!m_data)
        return false;
    return (m_data->size() - m_offset) >= requiredSize;
}

/*!
    Returns the current offset in the data buffer.
*/
int QOpcUaBinaryDataEncoding::offset() const
{
    return m_offset;
}

/*!
    Sets the current offset in the data buffer to \a offset.
    The first byte in the buffer has the offset 0.
*/
void QOpcUaBinaryDataEncoding::setOffset(int offset)
{
    m_offset = offset;
}

/*!
    Truncates the data buffer to the current \l offset().
    If the offset is behind the current buffer size, this method does nothing.

    This method can be used to roll back after an unsuccessful encode by setting
    the old offset and calling truncateBufferToOffset().
*/
void QOpcUaBinaryDataEncoding::truncateBufferToOffset()
{
    if (!m_data)
        return;

    if (m_offset < m_data->size() - 1)
        m_data->truncate(m_offset +  1);
}

template<>
bool QOpcUaBinaryDataEncoding::decode<bool>(bool &success)
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
QString QOpcUaBinaryDataEncoding::decode<QString>(bool &success)
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
QOpcUaQualifiedName QOpcUaBinaryDataEncoding::decode<QOpcUaQualifiedName>(bool &success)
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
QOpcUaLocalizedText QOpcUaBinaryDataEncoding::decode<QOpcUaLocalizedText>(bool &success)
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
QOpcUaEUInformation QOpcUaBinaryDataEncoding::decode<QOpcUaEUInformation>(bool &success)
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
QOpcUaRange QOpcUaBinaryDataEncoding::decode<QOpcUaRange>(bool &success)
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
QOpcUaComplexNumber QOpcUaBinaryDataEncoding::decode<QOpcUaComplexNumber>(bool &success)
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
QOpcUaDoubleComplexNumber QOpcUaBinaryDataEncoding::decode<QOpcUaDoubleComplexNumber>(bool &success)
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
QOpcUaAxisInformation QOpcUaBinaryDataEncoding::decode<QOpcUaAxisInformation>(bool &success)
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
QOpcUaXValue QOpcUaBinaryDataEncoding::decode<QOpcUaXValue>(bool &success)
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
QUuid QOpcUaBinaryDataEncoding::decode<QUuid>(bool &success)
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
QByteArray QOpcUaBinaryDataEncoding::decode<QByteArray>(bool &success)
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
QString QOpcUaBinaryDataEncoding::decode<QString, QOpcUa::Types::NodeId>(bool &success)
{
    quint8 identifierType = decode<quint8>(success);
    if (!success)
        return QString();

    identifierType &= ~(0x40 | 0x80); // Remove expanded node id flags

    quint16 namespaceIndex;

    if (identifierType == 0x00) {
        // encodingType 0x00 does not transfer the namespace index, it has to be zero
        // OPC UA 1.05 Part 6, Chapter 5.2.2.9, Section "Two Byte NodeId Binary DataEncoding"
        namespaceIndex = 0;
    } else if (identifierType == 0x01){
        // encodingType 0x01 transfers only one byte namespace index, has to be in range 0-255
        // OPC UA 1.05 Part 6, Chapter 5.2.2.9, Section "Four Byte NodeId Binary DataEncoding"
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
        return u"ns=%1;i=%2"_s.arg(namespaceIndex).arg(identifier);
    }
    case 0x01: {
        quint16 identifier = decode<quint16>(success);
        if (!success)
            return QString();
        return u"ns=%1;i=%2"_s.arg(namespaceIndex).arg(identifier);
    }
    case 0x02: {
        quint32 identifier = decode<quint32>(success);
        if (!success)
            return QString();
        return u"ns=%1;i=%2"_s.arg(namespaceIndex).arg(identifier);
    }
    case 0x03: {
        QString identifier = decode<QString>(success);
        if (!success)
            return QString();
        return u"ns=%1;s=%2"_s.arg(namespaceIndex).arg(identifier);
    }
    case 0x04: {
        QUuid identifier = decode<QUuid>(success);
        if (!success)
            return QString();
        return u"ns=%1;g=%2"_s.arg(namespaceIndex).arg(QStringView(identifier.toString()).mid(1, 36)); // Remove enclosing {...}
    }
    case 0x05: {
        QByteArray identifier = decode<QByteArray>(success);
        if (!success)
            return QString();
        return u"ns=%1;b=%2"_s.arg(namespaceIndex).arg(QString::fromLatin1(identifier.toBase64().constData()));
    }
    }

    success = false;
    return QString();
}

template <>
QOpcUaExpandedNodeId QOpcUaBinaryDataEncoding::decode<QOpcUaExpandedNodeId>(bool &success)
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
QDateTime QOpcUaBinaryDataEncoding::decode<QDateTime>(bool &success)
{
    qint64 timestamp = decode<qint64>(success);
    if (!success)
        return QDateTime();

    if (timestamp == 0 || timestamp == upperBound<qint64>())
        return QDateTime();

    // OPC UA 1.05 part 6, 5.2.2.5
    const QDateTime epochStart(QDate(1601, 1, 1), QTime(0, 0), QTimeZone::UTC);
    return epochStart.addMSecs(timestamp / 10000);
}

template <>
QOpcUa::UaStatusCode QOpcUaBinaryDataEncoding::decode<QOpcUa::UaStatusCode>(bool &success)
{
    quint32 value = decode<quint32>(success);
    if (!success)
        return QOpcUa::UaStatusCode(0);

    return QOpcUa::UaStatusCode(value);
}

template <>
QOpcUaExtensionObject QOpcUaBinaryDataEncoding::decode<QOpcUaExtensionObject>(bool &success)
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
QOpcUaArgument QOpcUaBinaryDataEncoding::decode<QOpcUaArgument>(bool &success)
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

template <>
QOpcUaStructureField QOpcUaBinaryDataEncoding::decode<QOpcUaStructureField>(bool &success)
{
    QOpcUaStructureField temp;

    temp.setName(decode<QString>(success));
    if (!success)
        return QOpcUaStructureField();

    temp.setDescription(decode<QOpcUaLocalizedText>(success));
    if (!success)
        return QOpcUaStructureField();

    temp.setDataType(decode<QString, QOpcUa::NodeId>(success));
    if (!success)
        return QOpcUaStructureField();

    temp.setValueRank(decode<qint32>(success));
    if (!success)
        return QOpcUaStructureField();

    temp.setArrayDimensions(decodeArray<quint32>(success));
    if (!success)
        return QOpcUaStructureField();

    temp.setMaxStringLength(decode<quint32>(success));
    if (!success)
        return QOpcUaStructureField();

    temp.setIsOptional(decode<bool>(success));
    if (!success)
        return QOpcUaStructureField();

    return temp;
}

template <>
QOpcUaStructureDefinition QOpcUaBinaryDataEncoding::decode<QOpcUaStructureDefinition>(bool &success)
{
    QOpcUaStructureDefinition temp;

    temp.setDefaultEncodingId(decode<QString, QOpcUa::NodeId>(success));
    if (!success)
        return QOpcUaStructureDefinition();

    temp.setBaseDataType(decode<QString, QOpcUa::NodeId>(success));
    if (!success)
        return QOpcUaStructureDefinition();

    temp.setStructureType(static_cast<QOpcUaStructureDefinition::StructureType>(decode<qint32>(success)));
    if (!success)
        return QOpcUaStructureDefinition();

    temp.setFields(decodeArray<QOpcUaStructureField>(success));
    if (!success)
        return QOpcUaStructureDefinition();

    return temp;
}

template <>
QOpcUaEnumField QOpcUaBinaryDataEncoding::decode<QOpcUaEnumField>(bool &success)
{
    QOpcUaEnumField temp;

    temp.setValue(decode<qint64>(success));
    if (!success)
        return QOpcUaEnumField();

    temp.setDisplayName(decode<QOpcUaLocalizedText>(success));
    if (!success)
        return QOpcUaEnumField();

    temp.setDescription(decode<QOpcUaLocalizedText>(success));
    if (!success)
        return QOpcUaEnumField();

    temp.setName(decode<QString>(success));
    if (!success)
        return QOpcUaEnumField();

    return temp;
}

template <>
QOpcUaEnumDefinition QOpcUaBinaryDataEncoding::decode<QOpcUaEnumDefinition>(bool &success)
{
    QOpcUaEnumDefinition temp;

    temp.setFields(decodeArray<QOpcUaEnumField>(success));
    if (!success)
        return QOpcUaEnumDefinition();

    return temp;
}

template <>
QOpcUaDiagnosticInfo QOpcUaBinaryDataEncoding::decode<QOpcUaDiagnosticInfo>(bool &success) {
    QOpcUaDiagnosticInfo temp;

    const auto encodingMask = decode<quint8>(success);
    if (!success)
        return {};

    temp.setHasSymbolicId(encodingMask & 0x01);
    temp.setHasNamespaceUri(encodingMask & 0x02);
    temp.setHasLocalizedText(encodingMask & 0x04);
    temp.setHasLocale(encodingMask & 0x08);
    temp.setHasAdditionalInfo(encodingMask & 0x10);
    temp.setHasInnerStatusCode(encodingMask & 0x20);
    temp.setHasInnerDiagnosticInfo(encodingMask & 0x40);

    if (temp.hasSymbolicId()) {
        temp.setSymbolicId(decode<qint32>(success));
        if (!success)
            return {};
    }

    if (temp.hasNamespaceUri()) {
        temp.setNamespaceUri(decode<qint32>(success));
        if (!success)
            return {};
    }

    if (temp.hasLocale()) {
        temp.setLocale(decode<qint32>(success));
        if (!success)
            return {};
    }

    if (temp.hasLocalizedText()) {
        temp.setLocalizedText(decode<qint32>(success));
        if (!success)
            return {};
    }

    if (temp.hasAdditionalInfo()) {
        temp.setAdditionalInfo(decode<QString>(success));
        if (!success)
            return {};
    }

    if (temp.hasInnerStatusCode()) {
        temp.setInnerStatusCode(decode<QOpcUa::UaStatusCode>(success));
        if (!success)
            return {};
    }

    if (temp.hasInnerDiagnosticInfo()) {
        temp.setInnerDiagnosticInfo(decode<QOpcUaDiagnosticInfo>(success));
        if (!success)
            return {};
    }

    return temp;
}

template <>
QOpcUaVariant QOpcUaBinaryDataEncoding::decode<QOpcUaVariant>(bool &success);

template <>
QOpcUaDataValue QOpcUaBinaryDataEncoding::decode<QOpcUaDataValue>(bool &success) {
    QOpcUaDataValue temp;

    const auto encodingMask = decode<quint8>(success);
    if (!success)
        return {};

    if (encodingMask & (1 << 0)) {
        temp.setValue(decode<QOpcUaVariant>(success));
        if (!success)
            return {};
    }

    if (encodingMask & (1 << 1)) {
        temp.setStatusCode(decode<QOpcUa::UaStatusCode>(success));
        if (!success)
            return {};
    }

    if (encodingMask & (1 << 2)) {
        temp.setSourceTimestamp(decode<QDateTime>(success));
        if (!success)
            return {};
    }

    if (encodingMask & (1 << 3)) {
        temp.setSourcePicoseconds(decode<quint16>(success));
        if (!success)
            return {};
    }

    if (encodingMask & (1 << 4)) {
        temp.setServerTimestamp(decode<QDateTime>(success));
        if (!success)
            return {};
    }

    if (encodingMask & (1 << 5)) {
        temp.setServerPicoseconds(decode<quint16>(success));
        if (!success)
            return {};
    }

    return temp;
}

template <>
QOpcUaVariant QOpcUaBinaryDataEncoding::decode<QOpcUaVariant>(bool &success) {
    QOpcUaVariant temp;

    const auto encodingMask = decode<quint8>(success);
    if (!success)
        return {};

    const bool hasArrayDimensions = encodingMask & (1 << 6);
    const bool isArray = encodingMask & (1 << 7);
    const auto type = QOpcUaVariant::ValueType(encodingMask & 0x1F);

    // Decode value

    QVariant value;

    switch (type) {
    case QOpcUaVariant::ValueType::Boolean:
        value = decodeValueArrayOrScalar<bool>(isArray, success);
        break;
    case QOpcUaVariant::ValueType::SByte:
        value = decodeValueArrayOrScalar<qint8>(isArray, success);
        break;
    case QOpcUaVariant::ValueType::Byte:
        value = decodeValueArrayOrScalar<quint8>(isArray, success);
        break;
    case QOpcUaVariant::ValueType::Int16:
        value = decodeValueArrayOrScalar<qint16>(isArray, success);
        break;
    case QOpcUaVariant::ValueType::UInt16:
        value = decodeValueArrayOrScalar<quint16>(isArray, success);
        break;
    case QOpcUaVariant::ValueType::Int32:
        value = decodeValueArrayOrScalar<qint32>(isArray, success);
        break;
    case QOpcUaVariant::ValueType::UInt32:
        value = decodeValueArrayOrScalar<quint32>(isArray, success);
        break;
    case QOpcUaVariant::ValueType::Int64:
        value = decodeValueArrayOrScalar<qint64>(isArray, success);
        break;
    case QOpcUaVariant::ValueType::UInt64:
        value = decodeValueArrayOrScalar<quint64>(isArray, success);
        break;
    case QOpcUaVariant::ValueType::Float:
        value = decodeValueArrayOrScalar<float>(isArray, success);
        break;
    case QOpcUaVariant::ValueType::Double:
        value = decodeValueArrayOrScalar<double>(isArray, success);
        break;
    case QOpcUaVariant::ValueType::String:
        value = decodeValueArrayOrScalar<QString>(isArray, success);
        break;
    case QOpcUaVariant::ValueType::DateTime:
        value = decodeValueArrayOrScalar<QDateTime>(isArray, success);
        break;
    case QOpcUaVariant::ValueType::Guid:
        value = decodeValueArrayOrScalar<QUuid>(isArray, success);
        break;
    case QOpcUaVariant::ValueType::ByteString:
        value = decodeValueArrayOrScalar<QByteArray>(isArray, success);
        break;
    case QOpcUaVariant::ValueType::XmlElement:
        value = decodeValueArrayOrScalar<QString>(isArray, success);
        break;
    case QOpcUaVariant::ValueType::NodeId:
        value = decodeValueArrayOrScalar<QString, QOpcUa::Types::NodeId>(isArray, success);
        break;
    case QOpcUaVariant::ValueType::ExpandedNodeId:
        value = decodeValueArrayOrScalar<QOpcUaExpandedNodeId>(isArray, success);
        break;
    case QOpcUaVariant::ValueType::StatusCode:
        value = decodeValueArrayOrScalar<QOpcUa::UaStatusCode>(isArray, success);
        break;
    case QOpcUaVariant::ValueType::QualifiedName:
        value = decodeValueArrayOrScalar<QOpcUaQualifiedName>(isArray, success);
        break;
    case QOpcUaVariant::ValueType::LocalizedText:
        value = decodeValueArrayOrScalar<QOpcUaLocalizedText>(isArray, success);
        break;
    case QOpcUaVariant::ValueType::ExtensionObject:
        value = decodeValueArrayOrScalar<QOpcUaExtensionObject>(isArray, success);
        break;
    case QOpcUaVariant::ValueType::DataValue:
        value = decodeValueArrayOrScalar<QOpcUaDataValue>(isArray, success);
        break;
    case QOpcUaVariant::ValueType::Variant:
        if (!isArray)
            return {}; // Variant must not contain a scalar variant as value
        value = QVariant::fromValue(decodeArray<QOpcUaVariant>(success));
        break;
    case QOpcUaVariant::ValueType::DiagnosticInfo:
        value = decodeValueArrayOrScalar<QOpcUaDiagnosticInfo>(isArray, success);
        break;
    default:
        break;
    }

    // 26-31 are reserved and shall be treated as ByteString when encountered
    // See OPC UA 1.05, Part 6, 5.2.2.16
    auto resultType = type;
    if (static_cast<quint16>(type) >= 26 && static_cast<quint16>(type) <= 31) {
        value = decodeValueArrayOrScalar<QByteArray>(isArray, success);
        resultType = QOpcUaVariant::ValueType::ByteString;
    }

    if (!success)
        return {};

    QList<qint32> arrayDimensions;
    if (hasArrayDimensions)
        arrayDimensions = decodeArray<qint32>(success);

    temp.setValue(resultType, value, arrayDimensions);

    return temp;
}

template<>
bool QOpcUaBinaryDataEncoding::encode<bool>(const bool &src)
{
    if (!m_data)
        return false;

    const quint8 value = src ? 1 : 0;
    m_data->append(reinterpret_cast<const char *>(&value), sizeof(value));
    return true;
}

template<>
bool QOpcUaBinaryDataEncoding::encode<QString>(const QString &src)
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
bool QOpcUaBinaryDataEncoding::encode<QOpcUaQualifiedName>(const QOpcUaQualifiedName &src)
{
    if (!encode<quint16>(src.namespaceIndex()))
        return false;
    if (!encode<QString>(src.name()))
        return false;
    return true;
}

template<>
bool QOpcUaBinaryDataEncoding::encode<QOpcUaLocalizedText>(const QOpcUaLocalizedText &src)
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
bool QOpcUaBinaryDataEncoding::encode<QOpcUaRange>(const QOpcUaRange &src)
{
    if (!encode<double>(src.low()))
        return false;
    if (!encode<double>(src.high()))
        return false;
    return true;
}

template <>
bool QOpcUaBinaryDataEncoding::encode<QOpcUaEUInformation>(const QOpcUaEUInformation &src)
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
bool QOpcUaBinaryDataEncoding::encode<QOpcUaComplexNumber>(const QOpcUaComplexNumber &src)
{
    if (!encode<float>(src.real()))
        return false;
    if (!encode<float>(src.imaginary()))
        return false;
    return true;
}

template <>
bool QOpcUaBinaryDataEncoding::encode<QOpcUaDoubleComplexNumber>(const QOpcUaDoubleComplexNumber &src)
{
    if (!encode<double>(src.real()))
        return false;
    if (!encode<double>(src.imaginary()))
        return false;
    return true;
}

template <>
bool QOpcUaBinaryDataEncoding::encode<QOpcUaAxisInformation>(const QOpcUaAxisInformation &src)
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
bool QOpcUaBinaryDataEncoding::encode<QOpcUaXValue>(const QOpcUaXValue &src)
{
    if (!encode<double>(src.x()))
        return false;
    if (!encode<float>(src.value()))
        return false;
    return true;
}

template <>
bool QOpcUaBinaryDataEncoding::encode<QUuid>(const QUuid &src)
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
bool QOpcUaBinaryDataEncoding::encode<QByteArray>(const QByteArray &src)
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
bool QOpcUaBinaryDataEncoding::encode<QString, QOpcUa::Types::NodeId>(const QString &src)
{
    if (!m_data)
        return false;

    quint16 index;
    QString identifier;
    char type;
    if (!QOpcUa::nodeIdStringSplit(src.isEmpty() ? u"ns=0;i=0"_s : src, &index, &identifier, &type))
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
            // OPC UA 1.05 Part 6, Chapter 5.2.2.9, Section "Two Byte NodeId Binary DataEncoding"
            if (!encoder.encode<quint8>(integerIdentifier))
                return false;
            encodingType = 0x00; // 8 bit numeric
            break;
        } else if (integerIdentifier <= 65535 && index <= 255) {
            // encodingType 0x01 transfers only one byte namespace index, has to be in range 0-255
            // OPC UA 1.05 Part 6, Chapter 5.2.2.9, Section "Four Byte NodeId Binary DataEncoding"
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
        // OPC UA 1.05 Part 6, Chapter 5.2.2.9, Section "Two Byte NodeId Binary DataEncoding"
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
bool QOpcUaBinaryDataEncoding::encode<QOpcUaExpandedNodeId>(const QOpcUaExpandedNodeId &src)
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
bool QOpcUaBinaryDataEncoding::encode<QDateTime>(const QDateTime &src)
{
    // OPC UA 1.05 part 6, 5.2.2.5
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
bool QOpcUaBinaryDataEncoding::encode<QOpcUa::UaStatusCode>(const QOpcUa::UaStatusCode &src)
{
    if (!encode<quint32>(src))
        return false;
    return true;
}

template <>
bool QOpcUaBinaryDataEncoding::encode<QOpcUaExtensionObject>(const QOpcUaExtensionObject &src)
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
bool QOpcUaBinaryDataEncoding::encode<QOpcUaArgument>(const QOpcUaArgument &src)
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

template <>
QOpcUaApplicationRecordDataType QOpcUaBinaryDataEncoding::decode<QOpcUaApplicationRecordDataType>(bool &success)
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
bool QOpcUaBinaryDataEncoding::encode<QOpcUaApplicationRecordDataType>(const QOpcUaApplicationRecordDataType &src)
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

template <>
bool QOpcUaBinaryDataEncoding::encode<QOpcUaStructureField>(const QOpcUaStructureField &src)
{
    if (!encode<QString>(src.name()))
        return false;
    if (!encode<QOpcUaLocalizedText>(src.description()))
        return false;
    if (!encode<QString, QOpcUa::NodeId>(src.dataType()))
        return false;
    if (!encode<qint32>(src.valueRank()))
        return false;
    if (!encodeArray<quint32>(src.arrayDimensions()))
        return false;
    if (!encode<quint32>(src.maxStringLength()))
        return false;
    if (!encode<bool>(src.isOptional()))
        return false;
    return true;
}

template <>
bool QOpcUaBinaryDataEncoding::encode<QOpcUaStructureDefinition>(const QOpcUaStructureDefinition &src)
{
    if (!encode<QString, QOpcUa::NodeId>(src.defaultEncodingId()))
        return false;
    if (!encode<QString, QOpcUa::NodeId>(src.baseDataType()))
        return false;
    if (!encode<qint32>(static_cast<qint32>(src.structureType())))
        return false;
    if (!encodeArray<QOpcUaStructureField>(src.fields()))
        return false;
    return true;
}

template <>
bool QOpcUaBinaryDataEncoding::encode<QOpcUaEnumField>(const QOpcUaEnumField &src)
{
    if (!encode<qint64>(src.value()))
        return false;
    if (!encode<QOpcUaLocalizedText>(src.displayName()))
        return false;
    if (!encode<QOpcUaLocalizedText>(src.description()))
        return false;
    if (!encode<QString>(src.name()))
        return false;
    return true;
}

template <>
bool QOpcUaBinaryDataEncoding::encode<QOpcUaEnumDefinition>(const QOpcUaEnumDefinition &src)
{
    if (!encodeArray<QOpcUaEnumField>(src.fields()))
        return false;
    return true;
}

template <>
bool QOpcUaBinaryDataEncoding::encode<QOpcUaDiagnosticInfo>(const QOpcUaDiagnosticInfo &src)
{
    quint8 encodingMask = 0;
    if (src.hasSymbolicId())
        encodingMask |= 0x01;
    if (src.hasNamespaceUri())
        encodingMask |= 0x02;
    if (src.hasLocalizedText())
        encodingMask |= 0x04;
    if (src.hasLocale())
        encodingMask |= 0x08;
    if (src.hasAdditionalInfo())
        encodingMask |= 0x10;
    if (src.hasInnerStatusCode())
        encodingMask |= 0x20;
    if (src.hasInnerDiagnosticInfo())
        encodingMask |= 0x40;

    if (!encode<quint8>(encodingMask))
        return false;

    if (src.hasSymbolicId()) {
        if (!encode<qint32>(src.symbolicId()))
            return false;
    }

    if (src.hasNamespaceUri()) {
        if (!encode<qint32>(src.namespaceUri()))
            return false;
    }

    if (src.hasLocale()) {
        if (!encode<qint32>(src.locale()))
            return false;
    }

    if (src.hasLocalizedText()) {
        if (!encode<qint32>(src.localizedText()))
            return false;
    }

    if (src.hasAdditionalInfo()) {
        if (!encode<QString>(src.additionalInfo()))
            return false;
    }

    if (src.hasInnerStatusCode()) {
        if (!encode<QOpcUa::UaStatusCode>(src.innerStatusCode()))
            return false;
    }

    if (src.hasInnerDiagnosticInfo()) {
        if (!encode<QOpcUaDiagnosticInfo>(src.innerDiagnosticInfo()))
            return false;
    }

    return true;
}

template <>
bool QOpcUaBinaryDataEncoding::encode<QOpcUaDataValue>(const QOpcUaDataValue &src)
{
    if (src.value().isValid() && !src.value().canConvert<QOpcUaVariant>()) {
        qWarning() << "Unable to convert DataValue value type != QOpcUaVariant";
        return false;
    }

    quint8 encodingMask = 0;
    if (src.value().isValid())
        encodingMask |= (1 << 0);
    if (src.statusCode() != QOpcUa::UaStatusCode::Good)
        encodingMask |= (1 << 1);
    if (src.sourceTimestamp().isValid())
        encodingMask |= (1 << 2);
    if (src.serverTimestamp().isValid())
        encodingMask |= (1 << 3);
    if (src.sourcePicoseconds())
        encodingMask |= (1 << 4);
    if (src.serverPicoseconds())
        encodingMask |= (1 << 5);

    if (!encode<quint8>(encodingMask))
        return false;

    // Encode value
    if (src.value().isValid()) {
        if (!encode<QOpcUaVariant>(src.value().value<QOpcUaVariant>()))
            return false;
    }

    if (src.statusCode() != QOpcUa::UaStatusCode::Good) {
        if (!encode<QOpcUa::UaStatusCode>(src.statusCode()))
            return false;
    }

    if (src.sourceTimestamp().isValid()) {
        if (!encode<QDateTime>(src.sourceTimestamp()))
            return false;
    }

    if (src.sourcePicoseconds()) {
        if (!encode<quint16>(src.sourcePicoseconds()))
            return false;
    }

    if (src.serverTimestamp().isValid()) {
        if (!encode<QDateTime>(src.serverTimestamp()))
            return false;
    }

    if (src.serverPicoseconds()) {
        if (!encode<quint16>(src.serverPicoseconds()))
            return false;
    }

    return true;
}

template <>
bool QOpcUaBinaryDataEncoding::encode<QOpcUaVariant>(const QOpcUaVariant &src)
{
    quint8 encodingMask = static_cast<quint8>(src.type());
    if (!src.arrayDimensions().isEmpty())
        encodingMask |= (1 << 6);
    if (src.isArray())
        encodingMask |= (1 << 7);

    if (!encode<quint8>(encodingMask))
        return false;

    bool success = true;
    switch (src.type()) {
    case QOpcUaVariant::ValueType::Boolean:
        success = encodeValueArrayOrScalar<bool>(src);
        break;
    case QOpcUaVariant::ValueType::SByte:
        success = encodeValueArrayOrScalar<qint8>(src);
        break;
    case QOpcUaVariant::ValueType::Byte:
        success = encodeValueArrayOrScalar<quint8>(src);
        break;
    case QOpcUaVariant::ValueType::Int16:
        success = encodeValueArrayOrScalar<qint16>(src);
        break;
    case QOpcUaVariant::ValueType::UInt16:
        success = encodeValueArrayOrScalar<quint16>(src);
        break;
    case QOpcUaVariant::ValueType::Int32:
        success = encodeValueArrayOrScalar<qint32>(src);
        break;
    case QOpcUaVariant::ValueType::UInt32:
        success = encodeValueArrayOrScalar<quint32>(src);
        break;
    case QOpcUaVariant::ValueType::Int64:
        success = encodeValueArrayOrScalar<qint64>(src);
        break;
    case QOpcUaVariant::ValueType::UInt64:
        success = encodeValueArrayOrScalar<quint32>(src);
        break;
    case QOpcUaVariant::ValueType::Float:
        success = encodeValueArrayOrScalar<float>(src);
        break;
    case QOpcUaVariant::ValueType::Double:
        success = encodeValueArrayOrScalar<double>(src);
        break;
    case QOpcUaVariant::ValueType::String:
        success = encodeValueArrayOrScalar<QString>(src);
        break;
    case QOpcUaVariant::ValueType::DateTime:
        success = encodeValueArrayOrScalar<QDateTime>(src);
        break;
    case QOpcUaVariant::ValueType::Guid:
        success = encodeValueArrayOrScalar<QUuid>(src);
        break;
    case QOpcUaVariant::ValueType::ByteString:
        success = encodeValueArrayOrScalar<QByteArray>(src);
        break;
    case QOpcUaVariant::ValueType::XmlElement:
        success = encodeValueArrayOrScalar<QString>(src);
        break;
    case QOpcUaVariant::ValueType::NodeId:
        success = encodeValueArrayOrScalar<QString, QOpcUa::Types::NodeId>(src);
        break;
    case QOpcUaVariant::ValueType::ExpandedNodeId:
        success = encodeValueArrayOrScalar<QOpcUaExpandedNodeId>(src);
        break;
    case QOpcUaVariant::ValueType::StatusCode:
        success = encodeValueArrayOrScalar<QOpcUa::UaStatusCode>(src);
        break;
    case QOpcUaVariant::ValueType::QualifiedName:
        success = encodeValueArrayOrScalar<QOpcUaQualifiedName>(src);
        break;
    case QOpcUaVariant::ValueType::LocalizedText:
        success = encodeValueArrayOrScalar<QOpcUaLocalizedText>(src);
        break;
    case QOpcUaVariant::ValueType::ExtensionObject:
        success = encodeValueArrayOrScalar<QOpcUaExtensionObject>(src);
        break;
    case QOpcUaVariant::ValueType::DataValue:
        success = encodeValueArrayOrScalar<QOpcUaDataValue>(src);
        break;
    case QOpcUaVariant::ValueType::Variant:
        if (!src.isArray()) {
            qWarning() << "Unable to convert Variant value, a Variant must not contain a scalar variant";
            return false;
        }
        success = encode<QOpcUaVariant>(src);
        break;
    case QOpcUaVariant::ValueType::DiagnosticInfo:
        success = encodeValueArrayOrScalar<QOpcUaDiagnosticInfo>(src);
        break;
    default:
        break;
    }

    if (!success)
        return false;

    if (!src.arrayDimensions().isEmpty()) {
        if (!encodeArray<qint32>(src.arrayDimensions()))
            return false;
    }

    return true;
}

QT_END_NAMESPACE
