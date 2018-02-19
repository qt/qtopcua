/****************************************************************************
**
** Copyright (C) 2016 basysKom GmbH, opensource@basyskom.com
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
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

#ifndef QOPCUACLIENTIMPL_P_H
#define QOPCUACLIENTIMPL_P_H

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

#include <QtOpcUa/qopcuaclient.h>
#include <QtOpcUa/qopcuaglobal.h>
#include <private/qopcuanodeimpl_p.h>

#include <QtCore/qobject.h>
#include <QtCore/qpointer.h>
#include <QtCore/qset.h>

QT_BEGIN_NAMESPACE

class QOpcUaNode;
class QOpcUaClient;
class QOpcUaBackend;
class QOpcUaMonitoringParameters;

class Q_OPCUA_EXPORT QOpcUaClientImpl : public QObject
{
    Q_OBJECT

public:
    QOpcUaClientImpl(QObject *parent = 0);
    virtual ~QOpcUaClientImpl();

    virtual void connectToEndpoint(const QUrl &url) = 0;
    virtual void disconnectFromEndpoint() = 0;
    virtual QOpcUaNode *node(const QString &nodeId) = 0;
    virtual QString backend() const = 0;

    void registerNode(QPointer<QOpcUaNodeImpl> obj);
    void unregisterNode(QPointer<QOpcUaNodeImpl> obj);

    void connectBackendWithClient(QOpcUaBackend *backend);

    QOpcUaClient *m_client;

private Q_SLOTS:
    void handleAttributesRead(uintptr_t handle, QVector<QOpcUaReadResult> attr, QOpcUa::UaStatusCode serviceResult);
    void handleAttributeWritten(uintptr_t handle, QOpcUa::NodeAttribute attr, const QVariant &value, QOpcUa::UaStatusCode statusCode);
    void handleAttributeUpdated(uintptr_t handle, const QOpcUaReadResult &value);
    void handleMonitoringEnableDisable(uintptr_t handle, QOpcUa::NodeAttribute attr, bool subscribe, QOpcUaMonitoringParameters status);
    void handleMonitoringStatusChanged(uintptr_t handle, QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameters items,
                                 QOpcUaMonitoringParameters param);
    void handleMethodCallFinished(uintptr_t handle, QString methodNodeId, QVariant result, QOpcUa::UaStatusCode statusCode);
    void handleBrowseFinished(uintptr_t handle, const QVector<QOpcUaReferenceDescription> &children, QOpcUa::UaStatusCode statusCode);

    void handleResolveBrowsePathFinished(uintptr_t handle, QVector<QOpcUa::QBrowsePathTarget> targets,
                                           QVector<QOpcUa::QRelativePathElement> path, QOpcUa::UaStatusCode status);

signals:
    void connected();
    void disconnected();
    void stateAndOrErrorChanged(QOpcUaClient::ClientState state,
                                QOpcUaClient::ClientError error);
private:
    Q_DISABLE_COPY(QOpcUaClientImpl)
    QHash<uintptr_t, QPointer<QOpcUaNodeImpl>> m_handles;
};

inline uint qHash(const QPointer<QOpcUaNodeImpl>& n)
{
    return ::qHash(n.data());
}

QT_END_NAMESPACE

#endif // QOPCUACLIENTIMPL_P_H
