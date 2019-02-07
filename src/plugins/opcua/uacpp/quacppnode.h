/******************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
******************************************************************************/

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
