/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
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

#ifndef QOPEN62541CLIENT_H
#define QOPEN62541CLIENT_H

#include <QTimer>
#include <private/qopcuaclientimpl_p.h>

#include <qopen62541.h>

QT_BEGIN_NAMESPACE

class Open62541AsyncBackend;

class QOpen62541Client : public QOpcUaClientImpl
{
    Q_OBJECT

public:
    explicit QOpen62541Client();
    ~QOpen62541Client();

    void connectToEndpoint(const QUrl &url) override;
    void secureConnectToEndpoint(const QUrl &url) override;
    void disconnectFromEndpoint() override;

    QOpcUaNode *node(const QString &nodeId) override;
    QOpcUaSubscription *createSubscription(quint32 interval) override;

    QString backend() const override;

    UA_Client* nativeClient() const;
    bool isSecureConnectionSupported() const override { return false; }
private slots:

private:
    friend class QOpen62541Node;
    QThread *m_thread;
    Open62541AsyncBackend *m_backend;
};

QT_END_NAMESPACE

#endif // QOPEN62541CLIENT_H
