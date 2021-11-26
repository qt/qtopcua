/****************************************************************************
**
** Copyright (C) 2018 Unified Automation GmbH
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
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
    bool isKeyAndCertificateFileSet() const;

private:
    QSharedDataPointer<QOpcUaPkiConfigurationData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaPkiConfiguration)

#endif // QOPCUAPKICONFIGURATION_H
