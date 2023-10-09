// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

/*!
    Default constructs an extension object with no parameters set.
*/
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
    \since 5.13

    Sets the body of this extension object to \a encodedBody,
    sets the encoding of the body to \l ByteString and
    sets the type id of the encoded data to \a typeId.

    \sa setEncodedBody(), setEncoding(), setEncodingTypeId()
*/
void QOpcUaExtensionObject::setBinaryEncodedBody(const QByteArray &encodedBody, const QString &typeId)
{
    setEncodedBody(encodedBody);
    setEncoding(Encoding::ByteString);
    setEncodingTypeId(typeId);
}

/*!
    \since 5.13

    Sets the body of this extension object to \a encodedBody,
    sets the encoding of the body to \l Xml and sets the type
    id of the encoded data to \a typeId.

    \sa setEncodedBody(), setEncoding(), setEncodingTypeId()
*/
void QOpcUaExtensionObject::setXmlEncodedBody(const QByteArray &encodedBody, const QString &typeId)
{
    setEncodedBody(encodedBody);
    setEncoding(Encoding::Xml);
    setEncodingTypeId(typeId);
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
