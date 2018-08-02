/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
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

#ifndef QOPCUANODE_H
#define QOPCUANODE_H

#include <QtOpcUa/qopcuabrowserequest.h>
#include <QtOpcUa/qopcuaglobal.h>
#include <QtOpcUa/qopcuamonitoringparameters.h>
#include <QtOpcUa/qopcuareferencedescription.h>
#include <QtOpcUa/qopcuatype.h>

#include <QtCore/qdatetime.h>
#include <QtCore/qdebug.h>
#include <QtCore/qvariant.h>
#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE

class QOpcUaNodePrivate;
class QOpcUaNodeImpl;
class QOpcUaClient;

class Q_OPCUA_EXPORT QOpcUaNode : public QObject
{
    Q_OBJECT

public:
    Q_DECLARE_PRIVATE(QOpcUaNode)

    static Q_DECL_CONSTEXPR QOpcUa::NodeAttributes mandatoryBaseAttributes();
    static Q_DECL_CONSTEXPR QOpcUa::NodeAttributes allBaseAttributes();
    typedef QMap<QOpcUa::NodeAttribute, QVariant> AttributeMap;

    QOpcUaNode(QOpcUaNodeImpl *impl, QOpcUaClient *client, QObject *parent = nullptr);
    virtual ~QOpcUaNode();

    bool readAttributes(QOpcUa::NodeAttributes attributes = mandatoryBaseAttributes());
    bool readAttributeRange(QOpcUa::NodeAttribute attribute, const QString &indexRange);
    bool readValueAttribute();
    QVariant attribute(QOpcUa::NodeAttribute attribute) const;
    QVariant value() const;
    QOpcUa::UaStatusCode attributeError(QOpcUa::NodeAttribute attribute) const;
    QOpcUa::UaStatusCode valueAttributeError() const;
    QDateTime sourceTimestamp(QOpcUa::NodeAttribute attribute) const;
    QDateTime serverTimestamp(QOpcUa::NodeAttribute attribute) const;
    bool writeAttribute(QOpcUa::NodeAttribute attribute, const QVariant &value, QOpcUa::Types type = QOpcUa::Types::Undefined);
    bool writeAttributeRange(QOpcUa::NodeAttribute attribute, const QVariant &value,
                        const QString &indexRange, QOpcUa::Types type = QOpcUa::Types::Undefined);
    bool writeAttributes(const AttributeMap &toWrite, QOpcUa::Types valueAttributeType = QOpcUa::Types::Undefined);
    bool writeValueAttribute(const QVariant &value, QOpcUa::Types type = QOpcUa::Types::Undefined);

    bool enableMonitoring(QOpcUa::NodeAttributes attr, const QOpcUaMonitoringParameters &settings);
    bool disableMonitoring(QOpcUa::NodeAttributes attr);
    bool modifyMonitoring(QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, const QVariant &value);
    QOpcUaMonitoringParameters monitoringStatus(QOpcUa::NodeAttribute attr);
    bool modifyEventFilter(const QOpcUaMonitoringParameters::EventFilter &eventFilter);
    bool modifyDataChangeFilter(QOpcUa::NodeAttribute attr, const QOpcUaMonitoringParameters::DataChangeFilter &filter);

    bool browseChildren(QOpcUa::ReferenceTypeId referenceType = QOpcUa::ReferenceTypeId::HierarchicalReferences,
                        QOpcUa::NodeClasses nodeClassMask = QOpcUa::NodeClass::Undefined);

    QString nodeId() const;
    QOpcUaClient *client() const;

    bool callMethod(const QString &methodNodeId, const QVector<QOpcUa::TypedVariant> &args = QVector<QOpcUa::TypedVariant>());

    bool resolveBrowsePath(const QVector<QOpcUa::QRelativePathElement> &path);

    bool browse(const QOpcUaBrowseRequest &request);

Q_SIGNALS:
    void attributeRead(QOpcUa::NodeAttributes attributes);
    void attributeWritten(QOpcUa::NodeAttribute attribute, QOpcUa::UaStatusCode statusCode);
    void attributeUpdated(QOpcUa::NodeAttribute attr, QVariant value);
    void eventOccurred(QVariantList eventFields);

    void monitoringStatusChanged(QOpcUa::NodeAttribute attr, QOpcUaMonitoringParameters::Parameters items,
                           QOpcUa::UaStatusCode statusCode);
    void enableMonitoringFinished(QOpcUa::NodeAttribute attr, QOpcUa::UaStatusCode statusCode);
    void disableMonitoringFinished(QOpcUa::NodeAttribute attr, QOpcUa::UaStatusCode statusCode);
    void methodCallFinished(QString methodNodeId, QVariant result, QOpcUa::UaStatusCode statusCode);
    void browseFinished(QVector<QOpcUaReferenceDescription> children, QOpcUa::UaStatusCode statusCode);
    void resolveBrowsePathFinished(QVector<QOpcUa::QBrowsePathTarget> targets,
                                     QVector<QOpcUa::QRelativePathElement> path, QOpcUa::UaStatusCode statusCode);

private:
    Q_DISABLE_COPY(QOpcUaNode)
};

Q_OPCUA_EXPORT QDebug operator<<(QDebug dbg, const QOpcUaNode &node);

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaNode::AttributeMap)

inline Q_DECL_CONSTEXPR QOpcUa::NodeAttributes QOpcUaNode::mandatoryBaseAttributes()
{
    return QOpcUa::NodeAttribute::NodeId | QOpcUa::NodeAttribute::NodeClass |
            QOpcUa::NodeAttribute::BrowseName | QOpcUa::NodeAttribute::DisplayName;
}

inline Q_DECL_CONSTEXPR QOpcUa::NodeAttributes QOpcUaNode::allBaseAttributes()
{
    return QOpcUa::NodeAttribute::NodeId | QOpcUa::NodeAttribute::NodeClass | QOpcUa::NodeAttribute::BrowseName |
            QOpcUa::NodeAttribute::DisplayName | QOpcUa::NodeAttribute::Description | QOpcUa::NodeAttribute::WriteMask |
            QOpcUa::NodeAttribute::UserWriteMask;
}

#endif // QOPCUANODE_H
