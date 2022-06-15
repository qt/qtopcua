// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QUACPPNODE_H
#define QUACPPNODE_H

#include "quacppclient.h"

#include <private/qopcuanodeimpl_p.h>

#include <QtCore/QPointer>

#include <uanodeid.h>

QT_BEGIN_NAMESPACE

class QUACppNode : public QOpcUaNodeImpl
{
public:
    explicit QUACppNode(const UaNodeId nodeId, QUACppClient *client, const QString nodeIdString);
    ~QUACppNode() override;

    bool readAttributes(QOpcUa::NodeAttributes attr, const QString &indexRange) override;
    bool enableMonitoring(QOpcUa::NodeAttributes attr, const QOpcUaMonitoringParameters &settings) override;
    bool disableMonitoring(QOpcUa::NodeAttributes attr) override;
    bool modifyMonitoring(QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, const QVariant &value) override;
    bool browse(const QOpcUaBrowseRequest &request);

    QString nodeId() const override;

    bool writeAttribute(QOpcUa::NodeAttribute attribute, const QVariant &value, QOpcUa::Types type, const QString &indexRange) override;
    bool writeAttributes(const QOpcUaNode::AttributeMap &toWrite, QOpcUa::Types valueAttributeType) override;
    bool callMethod(const QString &methodNodeId, const QList<QOpcUa::TypedVariant> &args) override;

    bool resolveBrowsePath(const QList<QOpcUaRelativePathElement> &path) override;

    QOpcUaHistoryReadResponse *readHistoryRaw(const QDateTime &startTime, const QDateTime &endTime, quint32 numValues, bool returnBounds) override;

private:
    QPointer<QUACppClient> m_client;
    QString m_nodeIdString;
    UaNodeId m_nodeId;
    quint32 m_handle;
};

QT_END_NAMESPACE

#endif // QUACPPNODE_H
