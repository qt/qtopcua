// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUABROWSEREQUEST_H
#define QOPCUABROWSEREQUEST_H

#include <QtOpcUa/qopcuatype.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOpcUaBrowseRequestData;
class Q_OPCUA_EXPORT QOpcUaBrowseRequest
{
public:

    enum class BrowseDirection : quint32 {
        Forward = 0,
        Inverse = 1,
        Both = 2
    };

    QOpcUaBrowseRequest();
    QOpcUaBrowseRequest(const QOpcUaBrowseRequest &other);
    QOpcUaBrowseRequest &operator=(const QOpcUaBrowseRequest &rhs);
    ~QOpcUaBrowseRequest();

    QOpcUaBrowseRequest::BrowseDirection browseDirection() const;
    void setBrowseDirection(const QOpcUaBrowseRequest::BrowseDirection &browseDirection);

    QString referenceTypeId() const;
    void setReferenceTypeId(const QString &referenceTypeId);
    void setReferenceTypeId(QOpcUa::ReferenceTypeId referenceTypeId);

    bool includeSubtypes() const;
    void setIncludeSubtypes(bool includeSubtypes);

    QOpcUa::NodeClasses nodeClassMask() const;
    void setNodeClassMask(const QOpcUa::NodeClasses &nodeClassMask);

private:
    QSharedDataPointer<QOpcUaBrowseRequestData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaBrowseRequest)

#endif // QOPCUABROWSEREQUEST_H
