/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
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

#ifndef OPCUADATACHANGEFILTER
#define OPCUADATACHANGEFILTER

#include <QOpcUaMonitoringParameters>

QT_BEGIN_NAMESPACE

class OpcUaDataChangeFilter : public QObject {
    Q_OBJECT
    Q_PROPERTY(DataChangeTrigger trigger READ trigger WRITE setTrigger)
    Q_PROPERTY(DeadbandType deadbandType READ deadbandType WRITE setDeadbandType)
    Q_PROPERTY(double deadbandValue READ deadbandValue WRITE setDeadbandValue)

public:
    // Same as in QOpcUaMonitoringParameters::DataChangeFilter::DataChangeTrigger
    enum class DataChangeTrigger {
        Status = 0,
        StatusOrValue = 1,
        StatusOrValueOrTimestamp = 2
    };
    Q_ENUM(DataChangeTrigger)

    // Same as in QOpcUaMonitoringParameters::DataChangeFilter::DeadbandType
    enum class DeadbandType {
        None = 0,
        Absolute = 1,
        Percent = 2
    };
    Q_ENUM(DeadbandType)

    explicit OpcUaDataChangeFilter(QObject *parent = nullptr);
    OpcUaDataChangeFilter(const OpcUaDataChangeFilter &);
    OpcUaDataChangeFilter &operator=(const OpcUaDataChangeFilter &);
    bool operator==(const OpcUaDataChangeFilter &) const;
    ~OpcUaDataChangeFilter();

    DataChangeTrigger trigger() const;
    DeadbandType deadbandType() const;
    double deadbandValue() const;

    const QOpcUaMonitoringParameters::DataChangeFilter &filter() const;

signals:
    void filterChanged();

public slots:
    void setTrigger(DataChangeTrigger trigger);
    void setDeadbandType(DeadbandType deadbandType);
    void setDeadbandValue(double deadbandValue);

private:
    QOpcUaMonitoringParameters::DataChangeFilter m_filter;
};

QT_END_NAMESPACE

#endif // OPCUADATACHANGEFILTER
