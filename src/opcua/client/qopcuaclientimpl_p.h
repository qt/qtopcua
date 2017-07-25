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

#include <QtCore/qobject.h>
#include <QtCore/QFuture>

#include <QtOpcUa/qopcuaglobal.h>

QT_BEGIN_NAMESPACE

class QOpcUaNode;
class QOpcUaClientPrivate;
class QOpcUaSubscription;

class Q_OPCUA_EXPORT QOpcUaClientImpl : public QObject
{
    Q_OBJECT

public:
    QOpcUaClientImpl(QObject *parent = 0);
    virtual ~QOpcUaClientImpl();

    virtual void connectToEndpoint(const QUrl &url) = 0;
    virtual void secureConnectToEndpoint(const QUrl &url) = 0;
    virtual void disconnectFromEndpoint() = 0;
    virtual QOpcUaNode *node(const QString &nodeId) = 0;
    virtual bool isSecureConnectionSupported() const = 0;
    virtual QString backend() const = 0;

    virtual QOpcUaSubscription *createSubscription(quint32 interval) = 0;

    QOpcUaClientPrivate *m_clientPrivate;

signals:
    void connected();
    void disconnected();
private:
    Q_DISABLE_COPY(QOpcUaClientImpl)
};

QT_END_NAMESPACE

#endif // QOPCUACLIENTIMPL_P_H
