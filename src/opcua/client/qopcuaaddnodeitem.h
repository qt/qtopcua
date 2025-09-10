// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUADDNODEITEM_H
#define QOPCUADDNODEITEM_H

#include <QtOpcUa/qopcuanodecreationattributes.h>
#include <QtOpcUa/qopcuatype.h>
#include <QtOpcUa/qopcuaexpandednodeid.h>
#include <QtOpcUa/qopcuaqualifiedname.h>

QT_BEGIN_NAMESPACE

class QOpcUaAddNodeItemData;
class Q_OPCUA_EXPORT QOpcUaAddNodeItem
{
public:
    QOpcUaAddNodeItem();
    QOpcUaAddNodeItem(const QOpcUaAddNodeItem &);
    QOpcUaAddNodeItem &operator=(const QOpcUaAddNodeItem &);
    ~QOpcUaAddNodeItem();

    QOpcUaExpandedNodeId parentNodeId() const;
    void setParentNodeId(const QOpcUaExpandedNodeId &parentNodeId);

    QString referenceTypeId() const;
    void setReferenceTypeId(const QString &referenceTypeId);

    QOpcUaExpandedNodeId requestedNewNodeId() const;
    void setRequestedNewNodeId(const QOpcUaExpandedNodeId &requestedNewNodeId);

    QOpcUaQualifiedName browseName() const;
    void setBrowseName(const QOpcUaQualifiedName &browseName);

    QOpcUa::NodeClass nodeClass() const;
    void setNodeClass(const QOpcUa::NodeClass &nodeClass);

    QOpcUaNodeCreationAttributes nodeAttributes() const;
    QOpcUaNodeCreationAttributes &nodeAttributesRef();
    void setNodeAttributes(const QOpcUaNodeCreationAttributes &nodeAttributes);

    QOpcUaExpandedNodeId typeDefinition() const;
    void setTypeDefinition(const QOpcUaExpandedNodeId &typeDefinition);

private:
    QSharedDataPointer<QOpcUaAddNodeItemData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaAddNodeItem)

#endif // QOPCUADDNODEITEM_H
