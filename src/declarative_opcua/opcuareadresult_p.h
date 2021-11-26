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
