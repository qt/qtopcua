/******************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
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
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

#ifndef QUACPPNODE_H
#define QUACPPNODE_H

#include "quacppclient.h"

#include <private/qopcuanodeimpl_p.h>

#include <QtCore/QPointer>

#include <uabase/uanodeid.h>

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
    bool callMethod(const QString &methodNodeId, const QVector<QOpcUa::TypedVariant> &args) override;

    bool resolveBrowsePath(const QVector<QOpcUa::QRelativePathElement> &path) override;

private:
    QPointer<QUACppClient> m_client;
    QString m_nodeIdString;
    UaNodeId m_nodeId;
    quint32 m_handle;
};

QT_END_NAMESPACE

#endif // QUACPPNODE_H
