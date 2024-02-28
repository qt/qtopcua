// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUA_OPCUAVALUENODE_P_H
#define QOPCUA_OPCUAVALUENODE_P_H

#include <private/opcuanode_p.h>
#include <private/opcuadatachangefilter_p.h>

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

class OpcUaValueNode : public OpcUaNode
{
    Q_OBJECT
    Q_DISABLE_COPY(OpcUaValueNode)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QOpcUa::Types valueType READ valueType WRITE setValueType)
    Q_PROPERTY(QDateTime serverTimestamp READ serverTimestamp)
    Q_PROPERTY(QDateTime sourceTimestamp READ sourceTimestamp)
    Q_PROPERTY(bool monitored READ monitored WRITE setMonitored NOTIFY monitoredChanged)
    Q_PROPERTY(double publishingInterval READ publishingInterval WRITE setPublishingInterval NOTIFY publishingIntervalChanged)
    Q_PROPERTY(OpcUaDataChangeFilter *filter READ filter WRITE setFilter NOTIFY filterChanged)

    QML_NAMED_ELEMENT(ValueNode)
    QML_ADDED_IN_VERSION(5, 12)

public:
    OpcUaValueNode(QObject *parent = nullptr);
    ~OpcUaValueNode();
    QVariant value() const;

    QDateTime serverTimestamp() const;
    QDateTime sourceTimestamp() const;
    bool monitored() const;
    double publishingInterval() const;
    QOpcUa::Types valueType() const;
    OpcUaDataChangeFilter *filter() const;
    void setFilter(OpcUaDataChangeFilter *filter);

public slots:
    void setValue(const QVariant &);
    void setMonitored(bool monitored);
    void setPublishingInterval(double publishingInterval);
    void setValueType(QOpcUa::Types valueType);


signals:
    void valueChanged(const QVariant &value);
    void monitoredChanged(bool monitored);
    void publishingIntervalChanged(double publishingInterval);
    void dataChangeOccurred(const QVariant &value);
    void filterChanged();

private slots:
    void setupNode(const QString &absolutePath) override;
    void updateSubscription();
    void updateFilters() const;

private:
    bool checkValidity() override;
    bool m_monitored = true;
    bool m_monitoredState = false;
    double m_publishingInterval = 100;
    QOpcUa::Types m_valueType = QOpcUa::Types::Undefined;
    OpcUaDataChangeFilter *m_filter = nullptr;
};

QT_END_NAMESPACE

#endif // QOPCUA_OPCUAVALUENODE_P_H
