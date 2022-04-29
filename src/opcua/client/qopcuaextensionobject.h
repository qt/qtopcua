/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

#ifndef QOPCUAEXTENSIONOBJECT_H
#define QOPCUAEXTENSIONOBJECT_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

class QOpcUaExtensionObjectData;
class Q_OPCUA_EXPORT QOpcUaExtensionObject
{
public:
    enum Encoding {
        NoBody = 0,
        ByteString = 1,
        Xml = 2
    };

    QOpcUaExtensionObject();
    QOpcUaExtensionObject(const QOpcUaExtensionObject &);
    ~QOpcUaExtensionObject();
    QOpcUaExtensionObject &operator=(const QOpcUaExtensionObject &);
    bool operator==(const QOpcUaExtensionObject &rhs) const;
    operator QVariant() const;

    QString encodingTypeId() const;
    void setEncodingTypeId(const QString &encodingTypeId);

    QByteArray encodedBody() const;
    QByteArray &encodedBodyRef();
    void setEncodedBody(const QByteArray &encodedBody);

    void setBinaryEncodedBody(const QByteArray &encodedBody, const QString &typeId);
    void setXmlEncodedBody(const QByteArray &encodedBody, const QString &typeId);

    QOpcUaExtensionObject::Encoding encoding() const;
    void setEncoding(QOpcUaExtensionObject::Encoding encoding);

private:
    QSharedDataPointer<QOpcUaExtensionObjectData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaExtensionObject)

#endif // QOPCUAEXTENSIONOBJECT_H
