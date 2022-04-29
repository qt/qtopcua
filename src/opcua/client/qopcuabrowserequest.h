/****************************************************************************
**
** Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
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
