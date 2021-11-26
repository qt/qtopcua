/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
