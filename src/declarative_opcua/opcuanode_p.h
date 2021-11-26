/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
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

#pragma once

#include <private/opcuaconnection_p.h>
#include <private/opcuanodeid_p.h>
#include <private/universalnode_p.h>
#include <private/opcuaattributecache_p.h>
#include <private/opcuaeventfilter_p.h>
#include <private/opcuanodeidtype_p.h>

#include <QtOpcUa/qopcualocalizedtext.h>

#include <QDateTime>
#include <QObject>

#include <QtQml/qqml.h>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

QT_BEGIN_NAMESPACE

class OpcUaNode : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(OpcUaNode)
    Q_CLASSINFO("RegisterEnumClassesUnscoped", "false")

    Q_PROPERTY(OpcUaNodeIdType* nodeId READ nodeId WRITE setNodeId NOTIFY nodeIdChanged)
    Q_PROPERTY(OpcUaConnection* connection READ connection WRITE setConnection NOTIFY connectionChanged)
    Q_PROPERTY(bool readyToUse READ readyToUse NOTIFY readyToUseChanged)
    Q_PROPERTY(OpcUaNode::Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    Q_PROPERTY(OpcUaEventFilter *eventFilter READ eventFilter WRITE setEventFilter NOTIFY eventFilterChanged)

    // basic node properties
    Q_PROPERTY(QString browseName READ browseName WRITE setBrowseName NOTIFY browseNameChanged)
    Q_PROPERTY(QOpcUa::NodeClass nodeClass READ nodeClass NOTIFY nodeClassChanged)
    Q_PROPERTY(QOpcUaLocalizedText displayName READ displayName WRITE setDisplayName NOTIFY displayNameChanged)
    Q_PROPERTY(QOpcUaLocalizedText description READ description WRITE setDescription NOTIFY descriptionChanged)

    Q_ENUM(QOpcUa::NodeClass);

    QML_NAMED_ELEMENT(Node)
    QML_ADDED_IN_VERSION(5, 12)

public:
   enum class Status {
        Valid,
        InvalidNodeId,
        NoConnection,
        InvalidNodeType,
        InvalidClient,
        FailedToResolveNode,
        InvalidObjectNode,
        FailedToReadAttributes,
        FailedToSetupMonitoring,
        FailedToWriteAttribute,
        FailedToModifyMonitoring,
        FailedToDisableMonitoring
    };
   Q_ENUM(Status);

    OpcUaNode(QObject *parent = nullptr);
    ~OpcUaNode();
    OpcUaNodeIdType *nodeId() const;
    OpcUaConnection *connection();
    bool readyToUse() const;

    void setBrowseName(const QString &value);
    QString browseName();

    QOpcUa::NodeClass nodeClass();

    void setDisplayName(const QOpcUaLocalizedText &value);
    QOpcUaLocalizedText displayName();

    void setDescription(const QOpcUaLocalizedText &value);
    QOpcUaLocalizedText description();

    OpcUaNode::Status status() const;
    const QString &errorMessage() const;

    using QObject::eventFilter;
    OpcUaEventFilter *eventFilter() const;
    void setEventFilter(OpcUaEventFilter *eventFilter);

    Q_INVOKABLE QDateTime getSourceTimestamp(QOpcUa::NodeAttribute) const;
    Q_INVOKABLE QDateTime getServerTimestamp(QOpcUa::NodeAttribute) const;

    // This function is not exposed to QML
    const UniversalNode &resolvedNode() const;

    // This function is not exposed to QML
    QOpcUaNode* node() const;

public slots:
    void setNodeId(OpcUaNodeIdType *nodeId);
    void setConnection(OpcUaConnection *);

signals:
    void nodeIdChanged(OpcUaNodeIdType *nodeId);
    void connectionChanged(OpcUaConnection *);
    void nodeChanged();
    void readyToUseChanged();
    void browseNameChanged();
    void nodeClassChanged();
    void displayNameChanged();
    void descriptionChanged();
    void statusChanged();
    void errorMessageChanged();
    void eventFilterChanged();
    void eventOccurred(const QVariantList&values);

protected slots:
    virtual void setupNode(const QString &absoluteNodePath);
    void updateNode();
    void updateEventFilter();

protected:
    void setStatus(Status status, const QString &message = QString());
    void setAttributesToRead(QOpcUa::NodeAttributes attributes);
    QOpcUa::NodeAttributes attributesToRead() const;
    void retrieveAbsoluteNodePath(OpcUaNodeIdType *, std::function<void (const QString &)>);
    void setReadyToUse(bool value = true);
    virtual bool checkValidity();

    OpcUaNodeIdType *m_nodeId = nullptr;
    QOpcUaNode *m_node = nullptr;
    OpcUaConnection *m_connection = nullptr;
    QString m_absoluteNodePath; // not exposed
    bool m_readyToUse = false;
    UniversalNode m_resolvedNode;
    OpcUaAttributeCache m_attributeCache;
    QOpcUa::NodeAttributes m_attributesToRead;
    Status m_status;
    QString m_errorMessage;
    OpcUaEventFilter *m_eventFilter = nullptr;
    bool m_eventFilterActive = false;

    QMetaObject::Connection m_attributeUpdatedConnection;
    QMetaObject::Connection m_attributeReadConnection;
    QMetaObject::Connection m_enableMonitoringFinishedConnection;
    QMetaObject::Connection m_disableMonitoringFinishedConnection;
    QMetaObject::Connection m_monitoringStatusChangedConnection;
    QMetaObject::Connection m_eventOccurredConnection;
};

QT_END_NAMESPACE
