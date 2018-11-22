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

    QString clientCertificateLocation() const;
    void setClientCertificateLocation(const QString &value);

    QString privateKeyLocation() const;
    void setPrivateKeyLocation(const QString &value);

    QString trustListLocation() const;
    void setTrustListLocation(const QString &value);

    QString revocationListLocation() const;
    void setRevocationListLocation(const QString &value);

    QString issuerListLocation() const;
    void setIssuerListLocation(const QString &value);

    QString issuerRevocationListLocation() const;
    void setIssuerRevocationListLocation(const QString &value);

    QOpcUaApplicationIdentity applicationIdentity() const;

    bool isPkiValid() const; // Bad name, open for better ideas

private:
    QSharedDataPointer<QOpcUaPkiConfigurationData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaPkiConfiguration)

#endif // QOPCUAPKICONFIGURATION_H
