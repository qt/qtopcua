/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
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
