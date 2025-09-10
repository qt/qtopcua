// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
