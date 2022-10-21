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

#ifndef QOPCUAAPPLICATIONRECORDDATATYPE_H
#define QOPCUAAPPLICATIONRECORDDATATYPE_H

#include <QtOpcUa/qopcuaglobal.h>
#include <QtOpcUa/qopcualocalizedtext.h>
#include <QtOpcUa/qopcuaapplicationdescription.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

class QOpcUaApplicationRecordDataTypeData;
class Q_OPCUA_EXPORT QOpcUaApplicationRecordDataType
{
public:
    QOpcUaApplicationRecordDataType();
    QOpcUaApplicationRecordDataType(const QOpcUaApplicationRecordDataType &);
    QOpcUaApplicationRecordDataType &operator=(const QOpcUaApplicationRecordDataType &);
    bool operator==(const QOpcUaApplicationRecordDataType &rhs) const;
    operator QVariant() const;
    ~QOpcUaApplicationRecordDataType();

    void setApplicationId(const QString &nodeId);
    const QString &applicationId() const;

    void setApplicationType(QOpcUaApplicationDescription::ApplicationType applicationType);
    QOpcUaApplicationDescription::ApplicationType applicationType() const;

    void setApplicationUri(const QString &applicationUri);
    const QString &applicationUri() const;

    void setApplicationNames(const QList<QOpcUaLocalizedText> &applicationNames);
    const QList<QOpcUaLocalizedText> &applicationNames() const;

    void setProductUri(const QString &productUri);
    const QString &productUri() const;

    void setDiscoveryUrls(const QList<QString> &discoveryUrls);
    const QList<QString> &discoveryUrls() const;

    void setServerCapabilityIdentifiers(const QList<QString> &serverCapabilityIdentifiers);
    const QList<QString> &serverCapabilityIdentifiers() const;

private:
    QSharedDataPointer<QOpcUaApplicationRecordDataTypeData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaApplicationRecordDataType)

#endif // QOPCUAAPPLICATIONRECORDDATATYPE_H
