// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef OPCUAREADRESULT_P_H
#define OPCUAREADRESULT_P_H

#include <private/opcuastatus_p.h>

#include <QtOpcUa/qopcuatype.h>
#include <QObject>
#include <QDateTime>

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

class QOpcUaReadResult;
class QOpcUaClient;
class OpcUaReadResultData;
class OpcUaReadResult
{
    Q_GADGET
    Q_PROPERTY(QOpcUa::NodeAttribute attribute READ attribute)
    Q_PROPERTY(QString indexRange READ indexRange)
    Q_PROPERTY(QString nodeId READ nodeId)
    Q_PROPERTY(QString namespaceName READ namespaceName)
    Q_PROPERTY(const QDateTime &serverTimestamp READ serverTimestamp)
    Q_PROPERTY(const QDateTime &sourceTimestamp READ sourceTimestamp)
    Q_PROPERTY(QVariant value READ value)
    Q_PROPERTY(OpcUaStatus status READ status)

public:
    OpcUaReadResult();
    OpcUaReadResult(const OpcUaReadResult &other);
    OpcUaReadResult(const QOpcUaReadResult &other, const QOpcUaClient *client);
    OpcUaReadResult &operator=(const OpcUaReadResult &rhs);
    ~OpcUaReadResult();

    const QString &indexRange() const;
    const QString &nodeId() const;
    QOpcUa::NodeAttribute attribute() const;
    const QString &namespaceName() const;
    const QDateTime &serverTimestamp() const;
    const QDateTime &sourceTimestamp() const;
    const QVariant &value() const;

    OpcUaStatus status() const;

private:
    QSharedDataPointer<OpcUaReadResultData> data;
};


QT_END_NAMESPACE

Q_DECLARE_METATYPE(OpcUaReadResult)

#endif // OPCUAREADRESULT_P_H
