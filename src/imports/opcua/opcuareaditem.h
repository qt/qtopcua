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

#ifndef OPCUAREADITEM_H
#define OPCUAREADITEM_H

#include <QObject>
#include <QtCore/qshareddata.h>
#include "qopcuatype.h"

QT_BEGIN_NAMESPACE

class OpcUaReadItemData;
class OpcUaReadItem
{
    Q_GADGET
    Q_PROPERTY(QOpcUa::NodeAttribute attribute READ attribute  WRITE setAttribute)
    Q_PROPERTY(QString indexRange READ indexRange WRITE setIndexRange)
    Q_PROPERTY(QString nodeId READ nodeId WRITE setNodeId)
    Q_PROPERTY(QVariant ns READ namespaceIdentifier WRITE setNamespaceIdentifier)

public:
    OpcUaReadItem();
    OpcUaReadItem(const OpcUaReadItem &other);
    OpcUaReadItem &operator=(const OpcUaReadItem &rhs);
    ~OpcUaReadItem();

    const QString &indexRange() const;
    void setIndexRange(const QString &indexRange);

    const QString &nodeId() const;
    void setNodeId(const QString &nodeId);

    QOpcUa::NodeAttribute attribute() const;
    void setAttribute(QOpcUa::NodeAttribute attribute);

    const QVariant &namespaceIdentifier() const;
    void setNamespaceIdentifier(const QVariant &namespaceIdentifier);

private:
    QSharedDataPointer<OpcUaReadItemData> data;
};

class OpcUaReadItemFactory : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE OpcUaReadItem create();
};

QT_END_NAMESPACE

#endif // OPCUAREADITEM_H
