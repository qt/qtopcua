// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPEN62541NODE_H
#define QOPEN62541NODE_H

#include "qopen62541client.h"
#include <private/qopcuanodeimpl_p.h>

#include <QPointer>

QT_BEGIN_NAMESPACE

class QOpen62541Node : public QOpcUaNodeImpl
{
public:
    explicit QOpen62541Node(const UA_NodeId nodeId, QOpen62541Client *client, const QString nodeIdString);
    ~QOpen62541Node() override;

    bool readAttributes(QOpcUa::NodeAttributes attr, const QString &indexRange) override;
    bool enableMonitoring(QOpcUa::NodeAttributes attr, const QOpcUaMonitoringParameters &settings) override;
    bool disableMonitoring(QOpcUa::NodeAttributes attr) override;
    bool modifyMonitoring(QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, const QVariant &value) override;
    bool browse(const QOpcUaBrowseRequest &request) override;
    QString nodeId() const override;

    bool writeAttribute(QOpcUa::NodeAttribute attribute, const QVariant &value, QOpcUa::Types type, const QString &indexRange) override;
    bool writeAttributes(const QOpcUaNode::AttributeMap &toWrite, QOpcUa::Types valueAttributeType) override;
    bool callMethod(const QString &methodNodeId, const QList<QOpcUa::TypedVariant> &args) override;

    QOpcUaHistoryReadResponse *readHistoryRaw(const QDateTime &startTime, const QDateTime &endTime, quint32 numValues, bool returnBounds) override;

    bool resolveBrowsePath(const QList<QOpcUaRelativePathElement> &path) override;

private:
    QPointer<QOpen62541Client> m_client;
    QString m_nodeIdString;
    UA_NodeId m_nodeId;
};

QT_END_NAMESPACE

#endif // QOPEN62541NODE_H
