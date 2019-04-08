/****************************************************************************
**
** Copyright (C) 2018 Unified Automation GmbH
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

#ifndef QOPCUAPKICONFIGURATION_H
#define QOPCUAPKICONFIGURATION_H

#include <QtOpcUa/qopcuaglobal.h>

#include <QtCore/qmetatype.h>
#include <QtCore/qshareddata.h>

#include <QtOpcUa/qopcuaapplicationidentity.h>

QT_BEGIN_NAMESPACE

class QOpcUaPkiConfigurationData;
class QOpcUaApplicationIdentity;

class Q_OPCUA_EXPORT QOpcUaPkiConfiguration
{
public:
    QOpcUaPkiConfiguration();
    ~QOpcUaPkiConfiguration();
    QOpcUaPkiConfiguration(const QOpcUaPkiConfiguration &other);
    QOpcUaPkiConfiguration &operator=(const QOpcUaPkiConfiguration &rhs);

    QString clientCertificateFile() const;
    void setClientCertificateFile(const QString &value);

    QString privateKeyFile() const;
    void setPrivateKeyFile(const QString &value);

    QString trustListDirectory() const;
    void setTrustListDirectory(const QString &value);

    QString revocationListDirectory() const;
    void setRevocationListDirectory(const QString &value);

    QString issuerListDirectory() const;
    void setIssuerListDirectory(const QString &value);

    QString issuerRevocationListDirectory() const;
    void setIssuerRevocationListDirectory(const QString &value);

    QOpcUaApplicationIdentity applicationIdentity() const;

    bool isPkiValid() const; // Bad name, open for better ideas

private:
    QSharedDataPointer<QOpcUaPkiConfigurationData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaPkiConfiguration)

#endif // QOPCUAPKICONFIGURATION_H
