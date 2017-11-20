/****************************************************************************
**
** Copyright (C) 2017 basysKom GmbH, opensource@basyskom.com
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

#include <private/qopcuabinarydataencoding_p.h>

QT_BEGIN_NAMESPACE

template<typename T>
T QOpcUaBinaryDataEncoding::decode(const char *&ptr, size_t &bufferSize, bool &success)
{
    if (bufferSize >= sizeof(T)) {
        T temp = *reinterpret_cast<const T *>(ptr);
        ptr += sizeof(T);
        success = true;
        bufferSize -= sizeof(T);
        return temp;
    } else {
        success = false;
        return T(0);
    }
}

template<>
QString QOpcUaBinaryDataEncoding::decode(const char *&ptr, size_t &bufferSize, bool &success)
{
    if (bufferSize < sizeof(qint32)) {
        success = false;
        return QString();
    }

    const qint32 length = decode<qint32>(ptr, bufferSize, success);

    if (!success)
        return QString();

    if (length > 0 && bufferSize < static_cast<size_t>(length)) {
        success = false;
        return QString();
    }

    if (length > 0) {
        QString temp =  QString::fromUtf8(reinterpret_cast<const char *>(ptr), length);
        ptr += length;
        bufferSize -= length;
        success = true;
        return temp;
    } else { // Empty string
        success = true;
        return QString();
    }
}

template <>
QOpcUa::QLocalizedText QOpcUaBinaryDataEncoding::decode(const char *&ptr, size_t &bufferSize, bool &success)
{
    QOpcUa::QLocalizedText temp;
    quint8 encodingMask = decode<quint8>(ptr, bufferSize, success);
    if (!success)
        return QOpcUa::QLocalizedText();

    if (encodingMask & 0x01) {
        temp.locale = decode<QString>(ptr, bufferSize, success);
        if (!success)
            return QOpcUa::QLocalizedText();
    }
    if (encodingMask & 0x02) {
        temp.text = decode<QString>(ptr, bufferSize, success);
        if (!success)
            return QOpcUa::QLocalizedText();
    }
    return temp;
}

template <>
QOpcUa::QEUInformation QOpcUaBinaryDataEncoding::decode(const char *&ptr, size_t &bufferSize, bool &success)
{
    QOpcUa::QEUInformation temp;
    temp.namespaceUri = decode<QString>(ptr, bufferSize, success);
    if (!success)
        return QOpcUa::QEUInformation();
    temp.unitId = decode<qint32>(ptr, bufferSize, success);
    if (!success)
        return QOpcUa::QEUInformation();
    temp.displayName = decode<QOpcUa::QLocalizedText>(ptr, bufferSize, success);
    if (!success)
        return QOpcUa::QEUInformation();
    temp.description = decode<QOpcUa::QLocalizedText>(ptr, bufferSize, success);
    if (!success)
        return QOpcUa::QEUInformation();
    return temp;
}

template <>
QOpcUa::QRange QOpcUaBinaryDataEncoding::decode(const char *&ptr, size_t &bufferSize, bool &success)
{
    QOpcUa::QRange temp;
    temp.low = decode<double>(ptr, bufferSize, success);
    if (!success)
        return QOpcUa::QRange();
    temp.high = decode<double>(ptr, bufferSize, success);
    if (!success)
        return QOpcUa::QRange();
    return temp;
}

template <>
QOpcUa::QComplexNumber QOpcUaBinaryDataEncoding::decode(const char *&ptr, size_t &bufferSize, bool &success)
{
    QOpcUa::QComplexNumber temp;
    temp.real = decode<float>(ptr, bufferSize, success);
    if (!success)
        return QOpcUa::QComplexNumber();
    temp.imaginary = decode<float>(ptr, bufferSize, success);
    if (!success)
        return QOpcUa::QComplexNumber();
    return temp;
}

template <>
QOpcUa::QDoubleComplexNumber QOpcUaBinaryDataEncoding::decode(const char *&ptr, size_t &bufferSize, bool &success)
{
    QOpcUa::QDoubleComplexNumber temp;
    temp.real = decode<double>(ptr, bufferSize, success);
    if (!success)
        return QOpcUa::QDoubleComplexNumber();
    temp.imaginary = decode<double>(ptr, bufferSize, success);
    if (!success)
        return QOpcUa::QDoubleComplexNumber();
    return temp;
}

template <>
QOpcUa::QAxisInformation QOpcUaBinaryDataEncoding::decode(const char *&ptr, size_t &bufferSize, bool &success)
{
    QOpcUa::QAxisInformation temp;
    temp.engineeringUnits = decode<QOpcUa::QEUInformation>(ptr, bufferSize, success);
    if (!success)
        return QOpcUa::QAxisInformation();
    temp.eURange = decode<QOpcUa::QRange>(ptr, bufferSize, success);
    if (!success)
        return QOpcUa::QAxisInformation();
    temp.title = decode<QOpcUa::QLocalizedText>(ptr, bufferSize, success);
    if (!success)
        return QOpcUa::QAxisInformation();
    temp.axisScaleType = static_cast<QOpcUa::AxisScale>(decode<quint32>(ptr, bufferSize, success));
    if (!success)
        return QOpcUa::QAxisInformation();
    temp.axisSteps = decodeArray<double>(ptr, bufferSize, success);
    if (!success)
        return QOpcUa::QAxisInformation();
    return temp;
}

template <>
QOpcUa::QXValue QOpcUaBinaryDataEncoding::decode(const char *&ptr, size_t &bufferSize, bool &success)
{
    QOpcUa::QXValue temp;
    temp.x = decode<double>(ptr, bufferSize, success);
    if (!success)
        return QOpcUa::QXValue();
    temp.value = decode<float>(ptr, bufferSize, success);
    if (!success)
        return QOpcUa::QXValue();
    return temp;
}

template<typename T>
void QOpcUaBinaryDataEncoding::encode(const T &src, QByteArray &dst)
{
    dst.append(reinterpret_cast<const char *>(&src), sizeof(T));
}

template<>
void QOpcUaBinaryDataEncoding::encode<QString>(const QString &src, QByteArray &dst)
{
    QByteArray arr = src.toUtf8();
    encode<qint32>(arr.length() > 0 ? arr.length() : -1 , dst);
    if (arr.length())
        dst.append(arr.data(), arr.length());
}

template<>
void QOpcUaBinaryDataEncoding::encode<QOpcUa::QLocalizedText>(const QOpcUa::QLocalizedText &src, QByteArray &dst)
{
    quint8 mask = 0;
    if (src.locale.length() != 0)
        mask |= 0x01;
    if (src.text.length() != 0)
        mask |= 0x02;
    encode<quint8>(mask, dst);
    if (src.locale.length())
        encode(src.locale, dst);
    if (src.text.length())
        encode(src.text, dst);
}

template <>
void QOpcUaBinaryDataEncoding::encode<QOpcUa::QRange>(const QOpcUa::QRange &src, QByteArray &dst)
{
    encode<double>(src.low, dst);
    encode<double>(src.high, dst);
}

template <>
void QOpcUaBinaryDataEncoding::encode<QOpcUa::QEUInformation>(const QOpcUa::QEUInformation &src, QByteArray &dst)
{
    encode<QString>(src.namespaceUri, dst);
    encode<qint32>(src.unitId, dst);
    encode<QOpcUa::QLocalizedText>(src.displayName, dst);
    encode<QOpcUa::QLocalizedText>(src.description, dst);
}

template <>
void QOpcUaBinaryDataEncoding::encode<QOpcUa::QComplexNumber>(const QOpcUa::QComplexNumber &src, QByteArray &dst)
{
    encode<float>(src.real, dst);
    encode<float>(src.imaginary, dst);
}

template <>
void QOpcUaBinaryDataEncoding::encode<QOpcUa::QDoubleComplexNumber>(const QOpcUa::QDoubleComplexNumber &src, QByteArray &dst)
{
    encode<double>(src.real, dst);
    encode<double>(src.imaginary, dst);
}

template <>
void QOpcUaBinaryDataEncoding::encode<QOpcUa::QAxisInformation>(const QOpcUa::QAxisInformation &src, QByteArray &dst)
{
    encode<QOpcUa::QEUInformation>(src.engineeringUnits, dst);
    encode<QOpcUa::QRange>(src.eURange, dst);
    encode<QOpcUa::QLocalizedText>(src.title, dst);
    encode<quint32>(static_cast<quint32>(src.axisScaleType), dst);
    encodeArray<double>(src.axisSteps, dst);
}

template <>
void QOpcUaBinaryDataEncoding::encode<QOpcUa::QXValue>(const QOpcUa::QXValue &src, QByteArray &dst)
{
    encode<double>(src.x, dst);
    encode<float>(src.value, dst);
}

template<typename T>
QVector<T> QOpcUaBinaryDataEncoding::decodeArray(const char *&ptr, size_t &bufferSize, bool &success)
{
    QVector<T> temp;

    qint32 size = decode<qint32>(ptr, bufferSize, success);
    if (!success)
        return temp;

    for (int i = 0; i < size; ++i) {
        temp.push_back(decode<T>(ptr, bufferSize, success));
        if (!success)
            return QVector<T>();
    }

    return temp;
}

template<typename T>
void QOpcUaBinaryDataEncoding::encodeArray(const QVector<T> &src, QByteArray &dst)
{
    encode<qint32>(src.isEmpty() ? -1 : src.size(), dst);
    for (const auto &element : src)
        encode<T>(element, dst);
}

QT_END_NAMESPACE
