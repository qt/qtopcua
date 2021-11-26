/****************************************************************************
**
** Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
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

#ifndef QOPCUAADDREFERENCEITEM_H
#define QOPCUAADDREFERENCEITEM_H

#include <QtOpcUa/qopcuanodecreationattributes.h>
#include <QtOpcUa/qopcuatype.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOpcUaExpandedNodeId;

class QOpcUaAddReferenceItemData;
class Q_OPCUA_EXPORT QOpcUaAddReferenceItem
{
public:
    QOpcUaAddReferenceItem();
    QOpcUaAddReferenceItem(const QOpcUaAddReferenceItem &other);
    QOpcUaAddReferenceItem &operator=(const QOpcUaAddReferenceItem &rhs);
    ~QOpcUaAddReferenceItem();

    QString sourceNodeId() const;
    void setSourceNodeId(const QString &sourceNodeId);

    QString referenceTypeId() const;
    void setReferenceTypeId(const QString &referenceTypeId);

    bool isForwardReference() const;
    void setIsForwardReference(bool isForwardReference);

    QOpcUaExpandedNodeId targetNodeId() const;
    void setTargetNodeId(const QOpcUaExpandedNodeId &targetNodeId);

    QOpcUa::NodeClass targetNodeClass() const;
    void setTargetNodeClass(QOpcUa::NodeClass targetNodeClass);

    QString targetServerUri() const;
    void setTargetServerUri(const QString &targetServerUri);

private:
    QSharedDataPointer<QOpcUaAddReferenceItemData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaAddReferenceItem)

#endif // QOPCUAADDREFERENCEITEM_H
