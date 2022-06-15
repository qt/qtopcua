// Copyright (C) 2019 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUANODEIMPL_P_H
#define QOPCUANODEIMPL_P_H

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

#include <QtOpcUa/qopcuaglobal.h>
#include <QtOpcUa/qopcuabrowsepathtarget.h>
#include <QtOpcUa/qopcuamonitoringparameters.h>
#include <QtOpcUa/qopcuanode.h>
#include <QtOpcUa/qopcuareaditem.h>
#include <QtOpcUa/qopcuareadresult.h>
#include <QtOpcUa/qopcuarelativepathelement.h>
#include <QtOpcUa/qopcuatype.h>
#include <QtOpcUa/qopcuahistoryreadrawrequest.h>

#include <QtCore/qvariant.h>
#include <QtCore/private/qglobal_p.h>

QT_BEGIN_NAMESPACE

class Q_OPCUA_EXPORT QOpcUaNodeImpl : public QObject
{
    Q_OBJECT
public:
    QOpcUaNodeImpl();
    virtual ~QOpcUaNodeImpl();

    virtual bool readAttributes(QOpcUa::NodeAttributes attr, const QString &indexRange) = 0;
    virtual bool enableMonitoring(QOpcUa::NodeAttributes attr, const QOpcUaMonitoringParameters &settings) = 0;
    virtual bool disableMonitoring(QOpcUa::NodeAttributes attr) = 0;
    virtual bool browse(const QOpcUaBrowseRequest &request) = 0;
    virtual QString nodeId() const = 0;

    virtual bool writeAttribute(QOpcUa::NodeAttribute attribute, const QVariant &value, QOpcUa::Types type, const QString &indexRange) = 0;
    virtual bool writeAttributes(const QOpcUaNode::AttributeMap &toWrite, QOpcUa::Types valueAttributeType) = 0;
    virtual bool modifyMonitoring(QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item,
                                          const QVariant &value) = 0;

    virtual bool callMethod(const QString &methodNodeId, const QList<QOpcUa::TypedVariant> &args) = 0;
    virtual QOpcUaHistoryReadResponse *readHistoryRaw(const QDateTime &startTime, const QDateTime &endTime, quint32 numValues, bool returnBounds) = 0;

    virtual bool resolveBrowsePath(const QList<QOpcUaRelativePathElement> &path) = 0;

    quint64 handle() const;
    void setHandle(quint64 handle);

    bool registered() const;
    void setRegistered(bool registered);

Q_SIGNALS:
    void attributesRead(QList<QOpcUaReadResult> attr, QOpcUa::UaStatusCode serviceResult);
    void attributeWritten(QOpcUa::NodeAttribute attr, QVariant value, QOpcUa::UaStatusCode statusCode);
    void browseFinished(QList<QOpcUaReferenceDescription> children, QOpcUa::UaStatusCode statusCode);

    void dataChangeOccurred(QOpcUa::NodeAttribute attr, QOpcUaReadResult value);
    void eventOccurred(QVariantList eventFields);
    void monitoringEnableDisable(QOpcUa::NodeAttribute attr, bool subscribe, QOpcUaMonitoringParameters status);
    void monitoringStatusChanged(QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameters items,
                           QOpcUaMonitoringParameters param);
    void methodCallFinished(QString methodNodeId, QVariant result, QOpcUa::UaStatusCode statusCode);
    void resolveBrowsePathFinished(QList<QOpcUaBrowsePathTarget> targets,
                                     QList<QOpcUaRelativePathElement> path, QOpcUa::UaStatusCode status);
    void readHistoryRawFinished(QList<QOpcUaHistoryData> results, QOpcUa::UaStatusCode serviceResult);

private:
    quint64 m_handle;
    bool m_registered;
};

QT_END_NAMESPACE

#endif // QOPCUANODEIMPL_P_H
