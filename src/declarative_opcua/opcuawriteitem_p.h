// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef OPCUAWRITEITEM_P_H
#define OPCUAWRITEITEM_P_H

#include <private/opcuastatus_p.h>

#include <QObject>
#include <QDateTime>
#include <QtCore/qshareddata.h>

#include <QtOpcUa/qopcuatype.h>

#include <QtQml/qqml.h>

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

class OpcUaWriteItemData;
class OpcUaWriteItem
{
    Q_GADGET
    Q_PROPERTY(QString nodeId READ nodeId WRITE setNodeId)
    Q_PROPERTY(QVariant ns READ namespaceIdentifier WRITE setNamespaceIdentifier)
    Q_PROPERTY(QOpcUa::NodeAttribute attribute READ attribute WRITE setAttribute)
    Q_PROPERTY(QString indexRange READ indexRange WRITE setIndexRange)
    Q_PROPERTY(QVariant value READ value WRITE setValue)
    Q_PROPERTY(QOpcUa::Types valueType READ valueType WRITE setValueType)
    Q_PROPERTY(QDateTime sourceTimestamp READ sourceTimestamp WRITE setSourceTimestamp)
    Q_PROPERTY(QDateTime serverTimestamp READ serverTimestamp WRITE setServerTimestamp)
    Q_PROPERTY(OpcUaStatus::Status statusCode READ statusCode WRITE setStatusCode)

public:
    OpcUaWriteItem();
    OpcUaWriteItem(const OpcUaWriteItem &other);
    OpcUaWriteItem &operator=(const OpcUaWriteItem &rhs);
    ~OpcUaWriteItem();

    const QString &nodeId() const;
    void setNodeId(const QString &nodeId);

    const QVariant &namespaceIdentifier() const;
    void setNamespaceIdentifier(const QVariant &namespaceIdentifier);

    QOpcUa::NodeAttribute attribute() const;
    void setAttribute(QOpcUa::NodeAttribute attribute);

    const QString &indexRange() const;
    void setIndexRange(const QString &indexRange);

    const QVariant &value() const;
    void setValue(const QVariant &value);

    QOpcUa::Types valueType() const;
    void setValueType(QOpcUa::Types type);

    const QDateTime &sourceTimestamp() const;
    void setSourceTimestamp(const QDateTime &sourceTimestamp);

    const QDateTime &serverTimestamp() const;
    void setServerTimestamp(const QDateTime &serverTimestamp);

    OpcUaStatus::Status statusCode() const;
    bool hasStatusCode() const;
    void setStatusCode(OpcUaStatus::Status statusCode);

private:
    QSharedDataPointer<OpcUaWriteItemData> data;
};

class OpcUaWriteItemFactory : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(WriteItem)
    QML_SINGLETON
    QML_ADDED_IN_VERSION(5, 13)

public:
    Q_INVOKABLE OpcUaWriteItem create();
};

QT_END_NAMESPACE

#endif // OPCUAWRITEITEM_P_H
