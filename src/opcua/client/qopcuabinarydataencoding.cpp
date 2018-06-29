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

#include "qopcuabinarydataencoding.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaBinaryDataEncoding
    \inmodule QtOpcUa
    \brief QOpcUaBinaryDataEncoding is a partial implementation of the OPC UA binary data encoding described in OPC-UA part 6.

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
            \li QOpcUa::QQualifiedName
            \li QualifiedName
        \row
            \li QOpcUa::QLocalizedText
            \li LocalizedText
        \row
            \li QOpcUa::QEUInformation
            \li EUInformation
        \row
            \li QOpcUa::QRange
            \li Range
        \row
            \li QOpcUa::QComplexNumber
            \li ComplexNumber
        \row
            \li QOpcUa::QDoubleComplexNumber
            \li DoubleComplexNumber
        \row
            \li QOpcUa::QAxisInformation
            \li AxisInformation
        \row
            \li QOpcUa::QXValue
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
            \li QOpcUa::QExpandedNodeId
            \li ExpandedNodeId
        \row
            \li QOpcUa::QExtensionObject
            \li ExtensionObject
        \row
            \li QOpcUa::QArgument
            \li Argument
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
    \fn template<typename T, QOpcUa::Types OVERLAY> QVector<T> QOpcUaBinaryDataEncoding::decodeArray(bool &success)

    Decodes an array of type T from the data buffer.
    \a success is set to \c true if the decoding was successful, \c false if not.

    The decoded value is returned. If \a success is false, the returned value is invalid.

    \sa decode()
*/

/*!
    \fn template<typename T, QOpcUa::Types OVERLAY> bool QOpcUaBinaryDataEncoding::encodeArray(const QVector<T> &src)

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

    \a object must not be deleted as long as this binary data encoding oject is used.
*/
QOpcUaBinaryDataEncoding::QOpcUaBinaryDataEncoding(QOpcUa::QExtensionObject &object)
    : m_data(&object.encodedBodyRef())
{
}

bool QOpcUaBinaryDataEncoding::enoughData(int requiredSize)
{
    if (!m_data)
        return false;
    return (m_data->size() + m_offset) >= requiredSize;
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

QT_END_NAMESPACE
