/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
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
