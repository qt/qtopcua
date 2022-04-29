/****************************************************************************
**
** Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

#ifndef QOPCUAREADITEM_H
#define QOPCUAREADITEM_H

#include <QtOpcUa/qopcuatype.h>
#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOpcUaReadItemData;
class Q_OPCUA_EXPORT QOpcUaReadItem
{
public:
    QOpcUaReadItem();
    QOpcUaReadItem(const QOpcUaReadItem &other);
    QOpcUaReadItem(const QString &nodeId, QOpcUa::NodeAttribute attr = QOpcUa::NodeAttribute::Value,
                   const QString &indexRange = QString());
    QOpcUaReadItem &operator=(const QOpcUaReadItem &rhs);
    ~QOpcUaReadItem();

    QString nodeId() const;
    void setNodeId(const QString &nodeId);

    QOpcUa::NodeAttribute attribute() const;
    void setAttribute(QOpcUa::NodeAttribute attribute);

    QString indexRange() const;
    void setIndexRange(const QString &indexRange);

private:
    QSharedDataPointer<QOpcUaReadItemData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaReadItem)

#endif // QOPCUAREADITEM_H
