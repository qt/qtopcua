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

    void connectToEndpoint(const QUrl &url);
    void disconnectFromEndpoint();

    QScopedPointer<QOpcUaClientImpl> m_impl;
    QOpcUaClient::ClientState m_state;
    QOpcUaClient::ClientError m_error;
    QUrl m_url;
    bool m_enableNamespaceArrayAutoupdate;

    bool checkAndSetUrl(const QUrl &url);
    void setStateAndError(QOpcUaClient::ClientState state,
                          QOpcUaClient::ClientError error = QOpcUaClient::NoError);

    bool updateNamespaceArray();
    QStringList namespaceArray() const;
    void namespaceArrayUpdated(QOpcUa::NodeAttributes attr);
    void setupNamespaceArrayMonitoring();

private:
    Q_DECLARE_PUBLIC(QOpcUaClient)
    QStringList m_namespaceArray;
    QScopedPointer<QOpcUaNode> m_namespaceArrayNode;
    bool m_namespaceArrayAutoupdateEnabled;
    unsigned int m_namespaceArrayUpdateInterval;
};

QT_END_NAMESPACE

#endif // QOPCUACLIENT_P_H
