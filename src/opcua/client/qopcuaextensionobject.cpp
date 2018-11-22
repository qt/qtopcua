/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
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

#include "qopcuaextensionobject.h"
#include "qopcuatype.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaExtensionObject
    \inmodule QtOpcUa
    \brief The OPC UA ExtensionObject.

    This is the Qt OPC UA representation for an extension object.
    Extension objects are used as a container in OPC UA whenever a non-builtin type is stored
    in a Variant. It contains information about the type and encoding of the data as well as
    the data itself encoded with one of the encodings specified in OPC-UA part 6.
    Decoders are supposed to decode extension objects if they can handle the type. If the type
    is not supported by the decoder, the extension object is not decoded and decoding is left
    to the user.
*/

/*!
    \enum QOpcUaExtensionObject::Encoding

    Enumerates the possible encodings of the body.

    \value NoBody
    \value ByteString
    \value Xml
*/

class QOpcUaExtensionObjectData : public QSharedData
{
public:
    QString encodingTypeId;
    QByteArray encodedBody;
    QOpcUaExtensionObject::Encoding encoding{QOpcUaExtensionObject::Encoding::NoBody};
};

QOpcUaExtensionObject::QOpcUaExtensionObject()
    : data(new QOpcUaExtensionObjectData)
{
}

/*!
    Constructs an extension object from \a rhs.
*/
QOpcUaExtensionObject::QOpcUaExtensionObject(const QOpcUaExtensionObject &rhs)
    : data(rhs.data)
{
}

/*!
    Sets the values from \a rhs in this extension object.
*/
QOpcUaExtensionObject &QOpcUaExtensionObject::operator=(const QOpcUaExtensionObject &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Returns \c true if this extension object has the same value as \a rhs.
*/
bool QOpcUaExtensionObject::operator==(const QOpcUaExtensionObject &rhs) const
{
    return data->encoding == rhs.encoding() &&
            QOpcUa::nodeIdEquals(data->encodingTypeId, rhs.encodingTypeId()) &&
            data->encodedBody == rhs.encodedBody();
}

/*!
    Converts this extension object to \l QVariant.
*/
QOpcUaExtensionObject::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

QOpcUaExtensionObject::~QOpcUaExtensionObject()
{
}

/*!
    Returns the \l {QOpcUaExtensionObject::Encoding} {encoding} of the body.
*/
QOpcUaExtensionObject::Encoding QOpcUaExtensionObject::encoding() const
{
    return data->encoding;
}

/*!
    Sets the encoding of the body to \a encoding.
*/
void QOpcUaExtensionObject::setEncoding(QOpcUaExtensionObject::Encoding encoding)
{
    data->encoding = encoding;
}

/*!
    Returns the body of this extension object. It contains the encoded data.
*/
QByteArray QOpcUaExtensionObject::encodedBody() const
{
    return data->encodedBody;
}

/*!
    Returns a reference to the body of this extension object.
*/
QByteArray &QOpcUaExtensionObject::encodedBodyRef()
{
    return data->encodedBody;
}
/*!
    Sets the body of this extension object to \a encodedBody.
*/
void QOpcUaExtensionObject::setEncodedBody(const QByteArray &encodedBody)
{
    data->encodedBody = encodedBody;
}

/*!
    Returns the node id of the encoding for the type stored by this extension object, for example ns=0;i=886 for
    Range_Encoding_DefaultBinary. All encoding ids are listed in \l {https://opcfoundation.org/UA/schemas/1.03/NodeIds.csv}.
*/
QString QOpcUaExtensionObject::encodingTypeId() const
{
    return data->encodingTypeId;
}

/*!
    Sets the node id of the encoding for the type stored by this extension object to \a encodingTypeId.
*/
void QOpcUaExtensionObject::setEncodingTypeId(const QString &encodingTypeId)
{
    data->encodingTypeId = encodingTypeId;
}

QT_END_NAMESPACE
