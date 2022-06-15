// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef OPCUAREADITEM_P_H
#define OPCUAREADITEM_P_H

#include <QObject>
#include <QtCore/qshareddata.h>
#include "qopcuatype.h"

#include <QtQml/qqml.h>
#include <QtCore/private/qglobal_p.h>

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
    QML_NAMED_ELEMENT(ReadItem)
    QML_SINGLETON
    QML_ADDED_IN_VERSION(5, 13)

public:
    Q_INVOKABLE OpcUaReadItem create();
};

QT_END_NAMESPACE

#endif // OPCUAREADITEM_P_H
