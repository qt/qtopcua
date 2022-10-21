/****************************************************************************
**
** Copyright (C) 2016 basysKom GmbH, opensource@basyskom.com
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

#include <QtCore/qvariant.h>

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

private:
    quint64 m_handle;
    bool m_registered;
};

QT_END_NAMESPACE

#endif // QOPCUANODEIMPL_P_H
