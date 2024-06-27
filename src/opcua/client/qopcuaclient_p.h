// Copyright (C) 2016 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
#include <QtOpcUa/qopcuaconnectionsettings.h>
#include <private/qopcuaclientimpl_p.h>

#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qurl.h>
#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

class Q_OPCUA_EXPORT QOpcUaClientPrivate : public QObjectPrivate
{
public:
    Q_DECLARE_PUBLIC(QOpcUaClient)

    QOpcUaClientPrivate(QOpcUaClientImpl *impl);
    ~QOpcUaClientPrivate() override;

    void connectToEndpoint(const QOpcUaEndpointDescription &endpoint);
    void disconnectFromEndpoint();

    // The following members are read by the backend and must be available
    // until m_impl has been destroyed.
    QOpcUaConnectionSettings m_connectionSettings;
    QOpcUaAuthenticationInformation m_authenticationInformation;
    QOpcUaApplicationIdentity m_applicationIdentity;
    QOpcUaPkiConfiguration m_pkiConfig;

    QScopedPointer<QOpcUaClientImpl> m_impl;
    QOpcUaClient::ClientState m_state;
    QOpcUaClient::ClientError m_error;
    QOpcUaEndpointDescription m_endpoint;
    bool m_enableNamespaceArrayAutoupdate;

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
    QStringList m_namespaceArray;
    QScopedPointer<QOpcUaNode> m_namespaceArrayNode;
    bool m_namespaceArrayAutoupdateEnabled;
    unsigned int m_namespaceArrayUpdateInterval;
};

QT_END_NAMESPACE

#endif // QOPCUACLIENT_P_H
