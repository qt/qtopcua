// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUAGDSCLIENTPRIVATE_H
#define QOPCUAGDSCLIENTPRIVATE_H

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

#include <QOpcUaClient>
#include <QOpcUaAuthenticationInformation>
#include <QOpcUaApplicationRecordDataType>
#include <QOpcUaEndpointDescription>
#include <QOpcUaPkiConfiguration>
#include <private/qobject_p.h>
#include "qopcuagdsclient.h"
#include <QOpcUaX509CertificateSigningRequest>

QT_BEGIN_NAMESPACE

class QTimer;

#define ApplicationRecordDataType_Encoding_DefaultBinary 134

class QOpcUaGdsClientPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QOpcUaGdsClient)
public:
    QOpcUaGdsClientPrivate();
    ~QOpcUaGdsClientPrivate();

    void initializePrivateConnections();

    void setEndpoint(const QOpcUaEndpointDescription &url);
    const QOpcUaEndpointDescription &endpoint() const;

    void setBackend(const QString &);
    const QString &backend() const;

    void setPkiConfiguration(const QOpcUaPkiConfiguration &pkiConfig);
    const QOpcUaPkiConfiguration &pkiConfiguration() const;

    void setApplicationIdentity(const QOpcUaApplicationIdentity &appIdentity);
    const QOpcUaApplicationIdentity &applicationIdentity() const;

    void setApplicationRecord(const QOpcUaApplicationRecordDataType &appIdentity);
    const QOpcUaApplicationRecordDataType &applicationRecord() const;

    void setCertificateSigningRequestPresets(const QOpcUaX509DistinguishedName &dn, const QString &dns);
    const QOpcUaX509DistinguishedName &distinguishedNameCertificateSigningRequestPreset() const;
    const QString &dnsCertificateSigningRequestPreset() const;

    void setCertificateCheckInterval(int timeout);
    int certificateCheckInterval() const;

    void setTrustListUpdateInterval(int interval);
    int trustListUpdateInterval() const;

    void start();
    void unregisterApplication();
    QString applicationId() const;

    QOpcUaGdsClient::Error error() const;
    QOpcUaGdsClient::State state() const;


private:
    void setError(QOpcUaGdsClient::Error);
    void setState(QOpcUaGdsClient::State);
    void _q_handleDirectoryNodeMethodCallFinished(QString methodNodeId, QVariant result, QOpcUa::UaStatusCode statusCode);
    void _q_handleResolveBrowsePathFinished(QList<QOpcUaBrowsePathTarget> targets, QList<QOpcUaRelativePathElement> path, QOpcUa::UaStatusCode statusCode);
    void _q_certificateCheckTimeout();
    void _q_updateTrustList();

    void getApplication();
    void registerApplication();
    void resolveMethodNodes();
    void resolveDirectoryNode();
    void findRegisteredApplication();
    void getCertificateGroups();
    void resolveCertificateTypes();
    void getCertificateTypes();
    void getCertificateStatus();
    void startCertificateRequest();
    void finishCertificateRequest();
    void localCertificateCheck();
    void registrationDone();
    void restartWithCredentials();
    void handleUnregisterApplicationFinished(const QVariant &result, QOpcUa::UaStatusCode statusCode);
    void handleFinishRequestFinished(const QVariant &result, QOpcUa::UaStatusCode statusCode);
    void handleStartSigningRequestFinished(const QVariant &result, QOpcUa::UaStatusCode statusCode);
    void handleGetCertificateStatusFinished(const QVariant &result, QOpcUa::UaStatusCode statusCode);
    void handleGetCertificateGroupsFinished(const QVariant &result, QOpcUa::UaStatusCode statusCode);
    void handleRegisterApplicationFinished(const QVariant &result, QOpcUa::UaStatusCode statusCode);
    void handleFindApplicationsFinished(const QVariant &result, QOpcUa::UaStatusCode statusCode);
    void handleGetApplicationFinished(const QVariant &result, QOpcUa::UaStatusCode statusCode);
    void handleGetTrustListFinished(const QVariant &result, QOpcUa::UaStatusCode statusCode);
    QOpcUaX509CertificateSigningRequest createSigningRequest() const;

    QOpcUaClient *m_client = nullptr;
    QOpcUaEndpointDescription m_endpoint;
    QOpcUaPkiConfiguration m_pkiConfig;
    QOpcUaApplicationIdentity m_appIdentitiy;
    QOpcUaApplicationRecordDataType m_appRecord;
    QString m_backend;
    QStringList m_certificateGroups;
    QOpcUaGdsClient::Error m_error = QOpcUaGdsClient::Error::NoError;
    QOpcUaGdsClient::State m_state = QOpcUaGdsClient::State::Idle;
    QMap<QString, QString> m_directoryNodes;
    QOpcUaNode *m_directoryNode = nullptr;
    QOpcUaNode *m_certificateGroupNode = nullptr;
    QString m_certificateTypesNodeId;
    int m_gdsNamespaceIndex = -1;
    QOpcUaNode *m_certificateTypesNode = nullptr;
    QOpcUaNode *m_trustListNode = nullptr;
    QTimer *m_certificateFinishTimer = nullptr;
    QTimer *m_certificateCheckTimer = nullptr;
    QString m_certificateRequestId;
    QTimer *m_trustListUpdateTimer = nullptr;
    bool m_restartRequired = false;
    QString m_configFilePath;

    struct {
        QOpcUaX509DistinguishedName dn;
        QString dns;
    } csrPresets;
};

QT_END_NAMESPACE

#endif // QOPCUAGDSCLIENTPRIVATE_H

