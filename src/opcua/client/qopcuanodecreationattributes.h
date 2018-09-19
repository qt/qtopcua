/****************************************************************************
**
** Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QOPCUANODECREATIONATTRIBUTES_H
#define QOPCUANODECREATIONATTRIBUTES_H

#include <QtOpcUa/qopcuatype.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOpcUaNodeCreationAttributesPrivate;

class Q_OPCUA_EXPORT QOpcUaNodeCreationAttributes
{
public:
    QOpcUaNodeCreationAttributes();
    QOpcUaNodeCreationAttributes(const QOpcUaNodeCreationAttributes &other);
    QOpcUaNodeCreationAttributes &operator=(const QOpcUaNodeCreationAttributes &rhs);

    ~QOpcUaNodeCreationAttributes();

    QOpcUa::AccessLevel accessLevel() const;
    void setAccessLevel(QOpcUa::AccessLevel accessLevel);
    bool hasAccessLevel() const;

    QVector<quint32> arrayDimensions() const;
    void setArrayDimensions(const QVector<quint32> &arrayDimensions);
    bool hasArrayDimensions() const;

    bool containsNoLoops() const;
    void setContainsNoLoops(bool containsNoLoops);
    bool hasContainsNoLoops() const;

    QString dataTypeId() const;
    void setDataTypeId(const QString &dataTypeId);
    bool hasDataTypeId() const;

    QOpcUa::QLocalizedText description() const;
    void setDescription(const QOpcUa::QLocalizedText &description);
    bool hasDescription() const;

    QOpcUa::QLocalizedText displayName() const;
    void setDisplayName(const QOpcUa::QLocalizedText &displayName);
    bool hasDisplayName() const;

    QOpcUa::EventNotifier eventNotifier() const;
    void setEventNotifier(QOpcUa::EventNotifier eventNotifier);
    bool hasEventNotifier() const;

    bool executable() const;
    void setExecutable(bool executable);
    bool hasExecutable() const;

    bool historizing() const;
    void setHistorizing(bool historizing);
    bool hasHistorizing() const;

    QOpcUa::QLocalizedText inverseName() const;
    void setInverseName(const QOpcUa::QLocalizedText &inverseName);
    bool hasInverseName() const;

    bool isAbstract() const;
    void setIsAbstract(bool isAbstract);
    bool hasIsAbstract() const;

    double minimumSamplingInterval() const;
    void setMinimumSamplingInterval(double minimumSamplingInterval);
    bool hasMinimumSamplingInterval() const;

    bool symmetric() const;
    void setSymmetric(bool symmetric);
    bool hasSymmetric() const;

    QOpcUa::AccessLevel userAccessLevel() const;
    void setUserAccessLevel(QOpcUa::AccessLevel userAccessLevel);
    bool hasUserAccessLevel() const;

    bool userExecutable() const;
    void setUserExecutable(bool userExecutable);
    bool hasUserExecutable() const;

    QOpcUa::WriteMask userWriteMask() const;
    void setUserWriteMask(QOpcUa::WriteMask userWriteMask);
    bool hasUserWriteMask() const;

    qint32 valueRank() const;
    void setValueRank(qint32 valueRank);
    bool hasValueRank() const;

    QOpcUa::WriteMask writeMask() const;
    void setWriteMask(QOpcUa::WriteMask writeMask);
    bool hasWriteMask() const;

    QVariant value() const;
    void setValue(const QVariant &value, QOpcUa::Types type = QOpcUa::Types::Undefined);
    bool hasValue() const;

    QOpcUa::Types valueType() const;

private:
    QSharedDataPointer<QOpcUaNodeCreationAttributesPrivate> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaNodeCreationAttributes)

#endif // QOPCUANODECREATIONATTRIBUTES_H
