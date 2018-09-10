/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#pragma once

#include <QObject>
#include "qopcuatype.h"
#include "universalnode.h"
#include "opcuaattributecache.h"

QT_BEGIN_NAMESPACE

class QOpcUaNode;
class OpcUaConnection;
class OpcUaNodeIdType;

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

    // basic node properties
    Q_PROPERTY(QString browseName READ browseName WRITE setBrowseName NOTIFY browseNameChanged)
    Q_PROPERTY(QOpcUa::NodeClass nodeClass READ nodeClass NOTIFY nodeClassChanged)
    Q_PROPERTY(QOpcUa::QLocalizedText displayName READ displayName WRITE setDisplayName NOTIFY displayNameChanged)
    Q_PROPERTY(QOpcUa::QLocalizedText description READ description WRITE setDescription NOTIFY descriptionChanged)

    Q_ENUM(QOpcUa::NodeClass);

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
        FailedToSetupMonitoring
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

    void setDisplayName(const QOpcUa::QLocalizedText &value);
    QOpcUa::QLocalizedText displayName();

    void setDescription(const QOpcUa::QLocalizedText &value);
    QOpcUa::QLocalizedText description();

    OpcUaNode::Status status() const;
    const QString &errorMessage() const;

    // This function is not exposed to QML
    const UniversalNode &resolvedNode() const;

    // This function is not exposed to QML
    QOpcUaNode* node() const;

public slots:
    void setNodeId(OpcUaNodeIdType *nodeId);
    void setConnection(OpcUaConnection *);

signals:
    void nodeIdChanged(const OpcUaNodeIdType *nodeId);
    void connectionChanged(OpcUaConnection *);
    void nodeChanged();
    void readyToUseChanged();
    void browseNameChanged();
    void nodeClassChanged();
    void displayNameChanged();
    void descriptionChanged();
    void statusChanged();
    void errorMessageChanged();

protected slots:
    virtual void setupNode(const QString &absoluteNodePath);
    void updateNode();

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
};

QT_END_NAMESPACE
