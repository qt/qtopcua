// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef OPCUADATACHANGEFILTER_P_H
#define OPCUADATACHANGEFILTER_P_H

#include <QOpcUaMonitoringParameters>
#include <QtQml/qqml.h>
#include <QtCore/private/qglobal_p.h>

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

class OpcUaDataChangeFilter : public QObject {
    Q_OBJECT
    Q_PROPERTY(DataChangeTrigger trigger READ trigger WRITE setTrigger)
    Q_PROPERTY(DeadbandType deadbandType READ deadbandType WRITE setDeadbandType)
    Q_PROPERTY(double deadbandValue READ deadbandValue WRITE setDeadbandValue)

    QML_NAMED_ELEMENT(DataChangeFilter)
    QML_ADDED_IN_VERSION(5, 13)

public:
    // Same as in QOpcUaMonitoringParameters::DataChangeFilter::DataChangeTrigger
    enum class DataChangeTrigger {
        Status = 0,
        StatusOrValue = 1,
        StatusOrValueOrTimestamp = 2,
    };
    Q_ENUM(DataChangeTrigger)

    // Same as in QOpcUaMonitoringParameters::DataChangeFilter::DeadbandType
    enum class DeadbandType {
        None = 0,
        Absolute = 1,
        Percent = 2,
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

#endif // OPCUADATACHANGEFILTER_P_H
