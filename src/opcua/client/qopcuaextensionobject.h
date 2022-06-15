// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
