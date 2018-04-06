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

#ifndef QOPCUADDNODEITEM_H
#define QOPCUADDNODEITEM_H

#include <QtOpcUa/qopcuanodecreationattributes.h>
#include <QtOpcUa/qopcuatype.h>

QT_BEGIN_NAMESPACE

class QOpcUaAddNodeItemData;
class Q_OPCUA_EXPORT QOpcUaAddNodeItem
{
public:
    QOpcUaAddNodeItem();
    QOpcUaAddNodeItem(const QOpcUaAddNodeItem &);
    QOpcUaAddNodeItem &operator=(const QOpcUaAddNodeItem &);
    ~QOpcUaAddNodeItem();

    QOpcUa::QExpandedNodeId parentNodeId() const;
    void setParentNodeId(const QOpcUa::QExpandedNodeId &parentNodeId);

    QString referenceTypeId() const;
    void setReferenceTypeId(const QString &referenceTypeId);

    QOpcUa::QExpandedNodeId requestedNewNodeId() const;
    void setRequestedNewNodeId(const QOpcUa::QExpandedNodeId &requestedNewNodeId);

    QOpcUa::QQualifiedName browseName() const;
    void setBrowseName(const QOpcUa::QQualifiedName &browseName);

    QOpcUa::NodeClass nodeClass() const;
    void setNodeClass(const QOpcUa::NodeClass &nodeClass);

    QOpcUaNodeCreationAttributes nodeAttributes() const;
    QOpcUaNodeCreationAttributes &nodeAttributesRef();
    void setNodeAttributes(const QOpcUaNodeCreationAttributes &nodeAttributes);

    QOpcUa::QExpandedNodeId typeDefinition() const;
    void setTypeDefinition(const QOpcUa::QExpandedNodeId &typeDefinition);

private:
    QSharedDataPointer<QOpcUaAddNodeItemData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaAddNodeItem)

#endif // QOPCUADDNODEITEM_H
