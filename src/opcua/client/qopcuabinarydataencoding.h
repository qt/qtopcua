// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUABINARYDATAENCODING_H
#define QOPCUABINARYDATAENCODING_H

#include <QtOpcUa/qopcuaglobal.h>
#include <QtOpcUa/qopcuatype.h>
#include <QtOpcUa/qopcuavariant.h>

#include <QtCore/qendian.h>
#include <QtCore/qlist.h>

#include <limits>

QT_BEGIN_NAMESPACE

class QOpcUaApplicationRecordDataType;
class QOpcUaArgument;
class QOpcUaAxisInformation;
class QOpcUaComplexNumber;
class QOpcUaDataValue;
class QOpcUaDiagnosticInfo;
class QOpcUaDoubleComplexNumber;
class QOpcUaEnumDefinition;
class QOpcUaEnumField;
class QOpcUaEUInformation;
class QOpcUaExpandedNodeId;
class QOpcUaExtensionObject;
class QOpcUaLocalizedText;
class QOpcUaQualifiedName;
class QOpcUaRange;
class QOpcUaStructureDefinition;
class QOpcUaStructureField;
class QOpcUaXValue;

class QByteArray;
class QDateTime;
class QUuid;


// This class implements a subset of the OPC UA Binary DataEncoding defined in OPC UA 1.05 part 6, 5.2.
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

    template <typename T, QOpcUa::Types OVERLAY = QOpcUa::Types::Undefined>
    bool encodeValueArrayOrScalar(const QOpcUaVariant &var) {
        return var.isArray() ? encodeArray<T, OVERLAY>(var.value().value<QList<T>>())
                             : encode<T, OVERLAY>(var.value().value<T>());
    }

    template <typename T, QOpcUa::Types OVERLAY = QOpcUa::Types::Undefined>
    QVariant decodeValueArrayOrScalar(bool isArray, bool &success) {
        return isArray ? QVariant::fromValue(decodeArray<T, OVERLAY>(success)) :
                   QVariant::fromValue(decode<T, OVERLAY>(success));
    }

    QByteArray *m_data{nullptr};
    int m_offset{0};
};

template<typename T, QOpcUa::Types OVERLAY>
T QOpcUaBinaryDataEncoding::decode(bool &success)
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

template<typename T, QOpcUa::Types OVERLAY>
bool QOpcUaBinaryDataEncoding::encode(const T &src)
{
    static_assert(OVERLAY == QOpcUa::Types::Undefined, "Ambiguous types are only permitted for template specializations");
    static_assert(std::is_arithmetic<T>::value == true, "Non-numeric types are only permitted for template specializations");

    if (!m_data)
        return false;

    T temp = qToLittleEndian<T>(src);
    m_data->append(reinterpret_cast<const char *>(&temp), sizeof(T));
    return true;
}

template<typename T, QOpcUa::Types OVERLAY>
QList<T> QOpcUaBinaryDataEncoding::decodeArray(bool &success)
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
bool QOpcUaBinaryDataEncoding::encodeArray(const QList<T> &src)
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

template<typename T>
T QOpcUaBinaryDataEncoding::upperBound()
{
    // Use extra parentheses to prevent macro substitution for max() on windows
    return (std::numeric_limits<T>::max)();
}

template<>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::encode<bool>(const bool &src);

template<>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::encode<QString>(const QString &src);

template<>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::encode<QOpcUaQualifiedName>(const QOpcUaQualifiedName &src);

template<>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::encode<QOpcUaLocalizedText>(const QOpcUaLocalizedText &src);

template <>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::encode<QOpcUaRange>(const QOpcUaRange &src);

template <>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::encode<QOpcUaEUInformation>(const QOpcUaEUInformation &src);

template <>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::encode<QOpcUaComplexNumber>(const QOpcUaComplexNumber &src);

template <>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::encode<QOpcUaDoubleComplexNumber>(const QOpcUaDoubleComplexNumber &src);

template <>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::encode<QOpcUaAxisInformation>(const QOpcUaAxisInformation &src);

template <>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::encode<QOpcUaXValue>(const QOpcUaXValue &src);

template <>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::encode<QUuid>(const QUuid &src);

template <>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::encode<QByteArray>(const QByteArray &src);

template <>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::encode<QString, QOpcUa::Types::NodeId>(const QString &src);

template <>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::encode<QOpcUaExpandedNodeId>(const QOpcUaExpandedNodeId &src);

template <>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::encode<QDateTime>(const QDateTime &src);

template <>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::encode<QOpcUa::UaStatusCode>(const QOpcUa::UaStatusCode &src);

template <>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::encode<QOpcUaExtensionObject>(const QOpcUaExtensionObject &src);

template <>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::encode<QOpcUaArgument>(const QOpcUaArgument &src);

template <>
Q_OPCUA_EXPORT QOpcUaApplicationRecordDataType QOpcUaBinaryDataEncoding::decode<QOpcUaApplicationRecordDataType>(bool &success);

template <>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::encode<QOpcUaApplicationRecordDataType>(const QOpcUaApplicationRecordDataType &src);

template <>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::encode<QOpcUaStructureField>(const QOpcUaStructureField &src);

template <>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::encode<QOpcUaStructureDefinition>(const QOpcUaStructureDefinition &src);

template <>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::encode<QOpcUaEnumField>(const QOpcUaEnumField &src);

template <>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::encode<QOpcUaEnumDefinition>(const QOpcUaEnumDefinition &src);

template <>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::encode<QOpcUaDiagnosticInfo>(const QOpcUaDiagnosticInfo &src);

template <>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::encode<QOpcUaDataValue>(const QOpcUaDataValue &src);

template <>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::encode<QOpcUaVariant>(const QOpcUaVariant &src);

template <>
Q_OPCUA_EXPORT bool QOpcUaBinaryDataEncoding::decode<bool>(bool &success);

template <>
Q_OPCUA_EXPORT QString QOpcUaBinaryDataEncoding::decode<QString>(bool &success);

template <>
Q_OPCUA_EXPORT QOpcUaQualifiedName QOpcUaBinaryDataEncoding::decode<QOpcUaQualifiedName>(bool &success);

template <>
Q_OPCUA_EXPORT QOpcUaLocalizedText QOpcUaBinaryDataEncoding::decode<QOpcUaLocalizedText>(bool &success);

template <>
Q_OPCUA_EXPORT QOpcUaEUInformation QOpcUaBinaryDataEncoding::decode<QOpcUaEUInformation>(bool &success);

template <>
Q_OPCUA_EXPORT QOpcUaRange QOpcUaBinaryDataEncoding::decode<QOpcUaRange>(bool &success);

template <>
Q_OPCUA_EXPORT QOpcUaComplexNumber QOpcUaBinaryDataEncoding::decode<QOpcUaComplexNumber>(bool &success);

template <>
Q_OPCUA_EXPORT QOpcUaDoubleComplexNumber QOpcUaBinaryDataEncoding::decode<QOpcUaDoubleComplexNumber>(bool &success);

template <>
Q_OPCUA_EXPORT QOpcUaAxisInformation QOpcUaBinaryDataEncoding::decode<QOpcUaAxisInformation>(bool &success);

template <>
Q_OPCUA_EXPORT QOpcUaXValue QOpcUaBinaryDataEncoding::decode<QOpcUaXValue>(bool &success);

template <>
Q_OPCUA_EXPORT QUuid QOpcUaBinaryDataEncoding::decode<QUuid>(bool &success);

template <>
Q_OPCUA_EXPORT QByteArray QOpcUaBinaryDataEncoding::decode<QByteArray>(bool &success);

template <>
Q_OPCUA_EXPORT QString QOpcUaBinaryDataEncoding::decode<QString, QOpcUa::Types::NodeId>(bool &success);

template <>
Q_OPCUA_EXPORT QOpcUaExpandedNodeId QOpcUaBinaryDataEncoding::decode<QOpcUaExpandedNodeId>(bool &success);

template <>
Q_OPCUA_EXPORT QDateTime QOpcUaBinaryDataEncoding::decode<QDateTime>(bool &success);

template <>
Q_OPCUA_EXPORT QOpcUa::UaStatusCode QOpcUaBinaryDataEncoding::decode<QOpcUa::UaStatusCode>(bool &success);

template <>
Q_OPCUA_EXPORT QOpcUaExtensionObject QOpcUaBinaryDataEncoding::decode<QOpcUaExtensionObject>(bool &success);

template <>
Q_OPCUA_EXPORT QOpcUaArgument QOpcUaBinaryDataEncoding::decode<QOpcUaArgument>(bool &success);

template <>
Q_OPCUA_EXPORT QOpcUaStructureField QOpcUaBinaryDataEncoding::decode<QOpcUaStructureField>(bool &success);

template <>
Q_OPCUA_EXPORT QOpcUaStructureDefinition QOpcUaBinaryDataEncoding::decode<QOpcUaStructureDefinition>(bool &success);

template <>
Q_OPCUA_EXPORT QOpcUaEnumField QOpcUaBinaryDataEncoding::decode<QOpcUaEnumField>(bool &success);

template <>
Q_OPCUA_EXPORT QOpcUaEnumDefinition QOpcUaBinaryDataEncoding::decode<QOpcUaEnumDefinition>(bool &success);

template <>
Q_OPCUA_EXPORT QOpcUaDiagnosticInfo QOpcUaBinaryDataEncoding::decode<QOpcUaDiagnosticInfo>(bool &success);

template <>
Q_OPCUA_EXPORT QOpcUaVariant QOpcUaBinaryDataEncoding::decode<QOpcUaVariant>(bool &success);

template <>
Q_OPCUA_EXPORT QOpcUaDataValue QOpcUaBinaryDataEncoding::decode<QOpcUaDataValue>(bool &success);

QT_END_NAMESPACE

#endif // QOPCUABINARYDATAENCODING_H
