/****************************************************************************
**
** Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
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

#ifndef QOPCUAREFERENCEDESCRIPTION_H
#define QOPCUAREFERENCEDESCRIPTION_H

#include <QtOpcUa/qopcuatype.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOpcUaReferenceDescriptionPrivate;

class Q_OPCUA_EXPORT QOpcUaReferenceDescription
{
public:
    QOpcUaReferenceDescription();
    QOpcUaReferenceDescription(const QOpcUaReferenceDescription &other);
    QOpcUaReferenceDescription &operator=(const QOpcUaReferenceDescription &other);

    ~QOpcUaReferenceDescription();

    QOpcUa::ReferenceTypeId refType() const;
    void setRefType(QOpcUa::ReferenceTypeId refType);
    QString nodeId() const;
    void setNodeId(const QString &nodeId);
    QOpcUa::QQualifiedName browseName() const;
    void setBrowseName(const QOpcUa::QQualifiedName &browseName);
    QOpcUa::QLocalizedText displayName() const;
    void setDisplayName(const QOpcUa::QLocalizedText &displayName);
    QOpcUa::NodeClass nodeClass() const;
    void setNodeClass(QOpcUa::NodeClass nodeClass);

private:
    QSharedDataPointer<QOpcUaReferenceDescriptionPrivate> d_ptr;
};

Q_DECLARE_TYPEINFO(QOpcUaReferenceDescription, Q_MOVABLE_TYPE);

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaReferenceDescription)

#endif // QOPCUAREFERENCEDESCRIPTION_H
