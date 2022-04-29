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

#ifndef QOPCUAREFERENCEDESCRIPTION_H
#define QOPCUAREFERENCEDESCRIPTION_H

#include <QtOpcUa/qopcuatype.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOpcUaExpandedNodeId;
class QOpcUaQualifiedName;
class QOpcUaLocalizedText;

class QOpcUaReferenceDescriptionPrivate;
class Q_OPCUA_EXPORT QOpcUaReferenceDescription
{
public:
    QOpcUaReferenceDescription();
    QOpcUaReferenceDescription(const QOpcUaReferenceDescription &other);
    QOpcUaReferenceDescription &operator=(const QOpcUaReferenceDescription &other);

    ~QOpcUaReferenceDescription();

    QString refTypeId() const;
    void setRefTypeId(const QString &refTypeId);
    QOpcUaExpandedNodeId targetNodeId() const;
    void setTargetNodeId(const QOpcUaExpandedNodeId &targetNodeId);
    QOpcUaQualifiedName browseName() const;
    void setBrowseName(const QOpcUaQualifiedName &browseName);
    QOpcUaLocalizedText displayName() const;
    void setDisplayName(const QOpcUaLocalizedText &displayName);
    QOpcUa::NodeClass nodeClass() const;
    void setNodeClass(QOpcUa::NodeClass nodeClass);
    void setIsForwardReference(bool isForwardReference);
    bool isForwardReference() const;
    void setTypeDefinition(const QOpcUaExpandedNodeId &typeDefinition);
    QOpcUaExpandedNodeId typeDefinition() const;

private:
    QSharedDataPointer<QOpcUaReferenceDescriptionPrivate> d_ptr;
};

Q_DECLARE_TYPEINFO(QOpcUaReferenceDescription, Q_MOVABLE_TYPE);

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaReferenceDescription)

#endif // QOPCUAREFERENCEDESCRIPTION_H
