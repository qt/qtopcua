/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
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

#include "opcuadatachangefilter.h"

QT_BEGIN_NAMESPACE

OpcUaDataChangeFilter::OpcUaDataChangeFilter(QObject *parent)
    : QObject(parent)
{

}

OpcUaDataChangeFilter::OpcUaDataChangeFilter(const OpcUaDataChangeFilter &other)
    : QObject(nullptr)
    , m_filter(other.filter())
{
}

OpcUaDataChangeFilter::~OpcUaDataChangeFilter() = default;

OpcUaDataChangeFilter &OpcUaDataChangeFilter::operator=(const OpcUaDataChangeFilter &other)
{
    m_filter = other.filter();
    return *this;
}

bool OpcUaDataChangeFilter::operator==(const OpcUaDataChangeFilter &other) const
{
    return m_filter == other.filter();
}

OpcUaDataChangeFilter::DataChangeTrigger OpcUaDataChangeFilter::trigger() const
{
    return static_cast<OpcUaDataChangeFilter::DataChangeTrigger>(m_filter.trigger());
}

OpcUaDataChangeFilter::DeadbandType OpcUaDataChangeFilter::deadbandType() const
{
    return static_cast<OpcUaDataChangeFilter::DeadbandType>(m_filter.deadbandType());
}

double OpcUaDataChangeFilter::deadbandValue() const
{
    return m_filter.deadbandValue();
}

const QOpcUaMonitoringParameters::DataChangeFilter &OpcUaDataChangeFilter::filter() const
{
    return m_filter;
}

void OpcUaDataChangeFilter::setTrigger(DataChangeTrigger trigger)
{
    const auto newValue = static_cast<QOpcUaMonitoringParameters::DataChangeFilter::DataChangeTrigger>(trigger);

    if (m_filter.trigger() == newValue)
        return;
    m_filter.setTrigger(newValue);
    emit filterChanged();
}

void OpcUaDataChangeFilter::setDeadbandType(DeadbandType deadbandType)
{
    const auto newValue = static_cast<QOpcUaMonitoringParameters::DataChangeFilter::DeadbandType>(deadbandType);

    if (m_filter.deadbandType() == newValue)
        return;
    m_filter.setDeadbandType(newValue);
    emit filterChanged();
}

void OpcUaDataChangeFilter::setDeadbandValue(double deadbandValue)
{
    if (deadbandValue == m_filter.deadbandValue())
        return;
    m_filter.setDeadbandValue(deadbandValue);
    emit filterChanged();
}

QT_END_NAMESPACE
