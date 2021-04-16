/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
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

#ifndef QOPCUAGDSCLIENT_H
#define QOPCUAGDSCLIENT_H

#include <QtCore/QObject>
#include <QtOpcUa/qopcuaglobal.h>

QT_BEGIN_NAMESPACE

class QOpcUaApplicationIdentity;
class QOpcUaAuthenticationInformation;
class QOpcUaEndpointDescription;
class QOpcUaPkiConfiguration;
class QOpcUaGdsClientPrivate;
class QOpcUaApplicationRecordDataType;
class QOpcUaX509DistinguishedName;

class Q_OPCUA_EXPORT QOpcUaGdsClient : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QOpcUaGdsClient)

public:
    enum class Error {
        NoError,
        InvalidBackend,
        InvalidEndpoint,
        ConnectionError,
        DirectoryNodeNotFound,
        FailedToRegisterApplication,
        FailedToUnregisterApplication,
        FailedToGetCertificateStatus,
        FailedToGetCertificate,
    };
    Q_ENUM(Error)

    enum class State {
        Idle,
        BackendInstantiated,
        Connecting,
        Connected,
        RegisteringApplication,
        ApplicationRegistered,
        //
        Error,
    };
    Q_ENUM(State)

    QOpcUaGdsClient(QObject *parent = nullptr);
    virtual ~QOpcUaGdsClient();

    void setBackend(const QString &backend);
    const QString &backend() const;

    void setEndpoint(const QOpcUaEndpointDescription &endpoint);
    const QOpcUaEndpointDescription &endpoint() const;

    void setPkiConfiguration(const QOpcUaPkiConfiguration &pkiConfig);
    const QOpcUaPkiConfiguration &pkiConfiguration() const;

    void setApplicationIdentity(const QOpcUaApplicationIdentity &appIdentity);
    const QOpcUaApplicationIdentity &applicationIdentity() const;

    void setApplicationRecord(const QOpcUaApplicationRecordDataType &appRecord);
    const QOpcUaApplicationRecordDataType &applicationRecord() const;

    QString applicationId() const;

    void setCertificateSigningRequestPresets(const QOpcUaX509DistinguishedName &dn, const QString &dns);
    const QOpcUaX509DistinguishedName &distinguishedNameCertificateSigningRequestPreset() const;
    const QString &dnsCertificateSigningRequestPreset() const;

    void setCertificateCheckInterval(int interval);
    int certificateCheckInterval() const;

    void setTrustListUpdateInterval(int interval);
    int trustListUpdateInterval() const;

    Error error() const;
    State state() const;

    void start();
    void unregisterApplication();

Q_SIGNALS:
    void stateChanged(State state);
    void errorChanged(Error error);
    void applicationRegistered();
    void certificateGroupsReceived(QStringList certificateGroups);
    void certificateUpdateRequired();
    void certificateUpdated();
    void unregistered();
    void trustListUpdated();
    void authenticationRequired(QOpcUaAuthenticationInformation &authInfo);

private:
    Q_PRIVATE_SLOT(d_func(), void _q_handleDirectoryNodeMethodCallFinished(QString, QVariant, QOpcUa::UaStatusCode))
    Q_PRIVATE_SLOT(d_func(), void _q_handleResolveBrowsePathFinished(QList<QOpcUaBrowsePathTarget>, QList<QOpcUaRelativePathElement>, QOpcUa::UaStatusCode))
    Q_PRIVATE_SLOT(d_func(), void _q_certificateCheckTimeout())
    Q_PRIVATE_SLOT(d_func(), void _q_updateTrustList())
};

QT_END_NAMESPACE

#endif // QOPCUAGDSCLIENT_H

