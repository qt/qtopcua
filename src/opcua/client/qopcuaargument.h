/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
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

#ifndef QOPCUAARGUMENT_H
#define QOPCUAARGUMENT_H

#include <QtOpcUa/qopcuaglobal.h>
#include <QtOpcUa/qopcualocalizedtext.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QVariant;

class QOpcUaArgumentData;
class Q_OPCUA_EXPORT QOpcUaArgument
{
public:
    QOpcUaArgument();
    QOpcUaArgument(const QOpcUaArgument &rhs);
    QOpcUaArgument(const QString &name, const QString &dataTypeId, qint32 valueRank,
                   const QList<quint32> &arrayDimensions, const QOpcUaLocalizedText &description);
    QOpcUaArgument &operator=(const QOpcUaArgument &);
    bool operator==(const QOpcUaArgument &other) const;
    operator QVariant() const;
    ~QOpcUaArgument();

    QString name() const;
    void setName(const QString &name);

    QString dataTypeId() const;
    void setDataTypeId(const QString &dataTypeId);

    qint32 valueRank() const;
    void setValueRank(qint32 valueRank);

    QList<quint32> arrayDimensions() const;
    QList<quint32> &arrayDimensionsRef();
    void setArrayDimensions(const QList<quint32> &arrayDimensions);

    QOpcUaLocalizedText description() const;
    void setDescription(const QOpcUaLocalizedText &description);

private:
    QSharedDataPointer<QOpcUaArgumentData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaArgument)

#endif // QOPCUAARGUMENT_H
