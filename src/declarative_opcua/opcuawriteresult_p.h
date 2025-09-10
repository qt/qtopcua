// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef OPCUAWRITERESULT_P_H
#define OPCUAWRITERESULT_P_H

#include <private/opcuastatus_p.h>

#include <QObject>
#include <QDateTime>
#include <QtOpcUa/qopcuatype.h>

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

class QOpcUaWriteResult;
class QOpcUaClient;
class OpcUaWriteResultData;
class OpcUaWriteResult
{
    Q_GADGET
    Q_PROPERTY(QOpcUa::NodeAttribute attribute READ attribute)
    Q_PROPERTY(QString indexRange READ indexRange)
    Q_PROPERTY(QString nodeId READ nodeId)
    Q_PROPERTY(QString namespaceName READ namespaceName)
    Q_PROPERTY(OpcUaStatus status READ status)

public:
    OpcUaWriteResult();
    OpcUaWriteResult(const OpcUaWriteResult &other);
    OpcUaWriteResult(const QOpcUaWriteResult &other, const QOpcUaClient *client);
    OpcUaWriteResult &operator=(const OpcUaWriteResult &rhs);
    ~OpcUaWriteResult();

    const QString &indexRange() const;
    const QString &nodeId() const;
    QOpcUa::NodeAttribute attribute() const;
    const QString &namespaceName() const;

    OpcUaStatus status() const;

private:
    QSharedDataPointer<OpcUaWriteResultData> data;
};


QT_END_NAMESPACE

Q_DECLARE_METATYPE(OpcUaWriteResult)

#endif // OPCUAWRITERESULT_P_H
