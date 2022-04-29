/****************************************************************************
**
** Copyright (C) 2016 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
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
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

#ifndef QOPCUACLIENT_P_H
#define QOPCUACLIENT_P_H

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
#include <QtOpcUa/qopcuaauthenticationinformation.h>
#include <private/qopcuaclientimpl_p.h>

#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qurl.h>
#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

class Q_OPCUA_EXPORT QOpcUaClientPrivate : public QObjectPrivate
{
public:
    QOpcUaClientPrivate(QOpcUaClientImpl *impl);
    ~QOpcUaClientPrivate() override;

    void connectToEndpoint(const QOpcUaEndpointDescription &endpoint);
    void disconnectFromEndpoint();

    QScopedPointer<QOpcUaClientImpl> m_impl;
    QOpcUaClient::ClientState m_state;
    QOpcUaClient::ClientError m_error;
    QOpcUaEndpointDescription m_endpoint;
    bool m_enableNamespaceArrayAutoupdate;
    QOpcUaAuthenticationInformation m_authenticationInformation;

    bool checkAndSetUrl(const QUrl &url);
    void setStateAndError(QOpcUaClient::ClientState state,
                          QOpcUaClient::ClientError error = QOpcUaClient::NoError);

    bool updateNamespaceArray();
    QStringList namespaceArray() const;
    void namespaceArrayUpdated(QOpcUa::NodeAttributes attr);
    void setupNamespaceArrayMonitoring();

    void setApplicationIdentity(const QOpcUaApplicationIdentity &identity);
    QOpcUaApplicationIdentity applicationIdentity() const;

    void setPkiConfiguration(const QOpcUaPkiConfiguration &config);
    QOpcUaPkiConfiguration pkiConfiguration() const;

private:
    Q_DECLARE_PUBLIC(QOpcUaClient)
    QStringList m_namespaceArray;
    QScopedPointer<QOpcUaNode> m_namespaceArrayNode;
    bool m_namespaceArrayAutoupdateEnabled;
    unsigned int m_namespaceArrayUpdateInterval;
    QOpcUaApplicationIdentity m_applicationIdentity;
    QOpcUaPkiConfiguration m_pkiConfig;
};

QT_END_NAMESPACE

#endif // QOPCUACLIENT_P_H
