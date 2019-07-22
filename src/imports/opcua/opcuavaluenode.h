/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
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

#pragma once

#include "opcuanode.h"
#include <QDateTime>
#include "opcuadatachangefilter.h"

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
