// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUANODECREATIONATTRIBUTES_H
#define QOPCUANODECREATIONATTRIBUTES_H

#include <QtOpcUa/qopcuatype.h>
#include <QtOpcUa/qopcualocalizedtext.h>
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

    QList<quint32> arrayDimensions() const;
    void setArrayDimensions(const QList<quint32> &arrayDimensions);
    bool hasArrayDimensions() const;

    bool containsNoLoops() const;
    void setContainsNoLoops(bool containsNoLoops);
    bool hasContainsNoLoops() const;

    QString dataTypeId() const;
    void setDataTypeId(const QString &dataTypeId);
    bool hasDataTypeId() const;

    QOpcUaLocalizedText description() const;
    void setDescription(const QOpcUaLocalizedText &description);
    bool hasDescription() const;

    QOpcUaLocalizedText displayName() const;
    void setDisplayName(const QOpcUaLocalizedText &displayName);
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

    QOpcUaLocalizedText inverseName() const;
    void setInverseName(const QOpcUaLocalizedText &inverseName);
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
