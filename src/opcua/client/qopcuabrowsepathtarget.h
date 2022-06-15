// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUABROWSEPATHTARGET_H
#define QOPCUABROWSEPATHTARGET_H

#include <QtOpcUa/qopcuaglobal.h>
#include <QtOpcUa/qopcuaexpandednodeid.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOpcUaBrowsePathTargetData;
class Q_OPCUA_EXPORT QOpcUaBrowsePathTarget
{
public:
    QOpcUaBrowsePathTarget();
    QOpcUaBrowsePathTarget(const QOpcUaBrowsePathTarget &);
    QOpcUaBrowsePathTarget &operator=(const QOpcUaBrowsePathTarget &);
    bool operator==(const QOpcUaBrowsePathTarget &rhs) const;
    ~QOpcUaBrowsePathTarget();

    QOpcUaExpandedNodeId targetId() const;
    QOpcUaExpandedNodeId &targetIdRef();
    void setTargetId(const QOpcUaExpandedNodeId &targetId);

    quint32 remainingPathIndex() const;
    void setRemainingPathIndex(quint32 remainingPathIndex);

    bool isFullyResolved() const;

private:
    QSharedDataPointer<QOpcUaBrowsePathTargetData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaBrowsePathTarget)

#endif // QOPCUABROWSEPATHTARGET_H
