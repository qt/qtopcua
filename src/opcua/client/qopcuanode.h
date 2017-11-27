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

#include <QtOpcUa/qopcuaglobal.h>
#include <QtOpcUa/qopcuamonitoringparameters.h>
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

    // see OPC-UA Part 3, 5.2.3 & 8.30
    enum class NodeClass {
        Undefined = 0,
        Object = 1,
        Variable = 2,
        Method = 4,
        ObjectType = 8,
        VariableType = 16,
        ReferenceType = 32,
        DataType = 64,
        View = 128,
    };
    Q_ENUM(NodeClass)

    enum class NodeAttribute {
        None = 0,
        NodeId = (1 << 0),
        NodeClass = (1 << 1),
        BrowseName = (1 << 2),
        DisplayName = (1 << 3),
        Description = (1 << 4),
        WriteMask = (1 << 5),
        UserWriteMask = (1 << 6), // Base attributes, see part 4, 5.2.1
        IsAbstract = (1 << 7),
        Symmetric = (1 << 8),
        InverseName = (1 << 9),   // Reference attributes, see part 4, 5.3.1
        ContainsNoLoops = (1 << 10),
        EventNotifier = (1 << 11), // View attributes, see part 4, 5.4
        // Objects also add the EventNotifier attribute, see part 4, 5.5.1
        // ObjectType also add the IsAbstract attribute, see part 4, 5.5.2
        Value = (1 << 12),
        DataType = (1 << 13),
        ValueRank = (1 << 14),
        ArrayDimensions = (1 << 15),
        AccessLevel = (1 << 16),
        UserAccessLevel = (1 << 17),
        MinimumSamplingInterval = (1 << 18),
        Historizing = (1 << 19),   // Value attributes, see part 4, 5.6.2
        // VariableType also adds the Value, DataType, ValueRank, ArrayDimensions
        // and isAbstract attributes, see part 4, 5.6.5
        Executable = (1 << 20),
        UserExecutable = (1 << 21), // Method attributes, see part 4, 5.7
    };
    Q_ENUM(NodeAttribute)
    Q_DECLARE_FLAGS(NodeAttributes, NodeAttribute)

    static Q_DECL_CONSTEXPR QOpcUaNode::NodeAttributes mandatoryBaseAttributes();
    static Q_DECL_CONSTEXPR QOpcUaNode::NodeAttributes allBaseAttributes();
    typedef QMap<QOpcUaNode::NodeAttribute, QVariant> AttributeMap;

    QOpcUaNode(QOpcUaNodeImpl *impl, QOpcUaClient *client, QObject *parent = nullptr);
    virtual ~QOpcUaNode();

    bool readAttributes(QOpcUaNode::NodeAttributes attributes = mandatoryBaseAttributes());
    QVariant attribute(QOpcUaNode::NodeAttribute attribute) const;
    QOpcUa::UaStatusCode attributeError(QOpcUaNode::NodeAttribute attribute) const;
    bool writeAttribute(QOpcUaNode::NodeAttribute attribute, const QVariant &value, QOpcUa::Types type = QOpcUa::Types::Undefined);
    bool writeAttributes(const AttributeMap &toWrite, QOpcUa::Types valueAttributeType = QOpcUa::Types::Undefined);

    bool enableMonitoring(QOpcUaNode::NodeAttributes attr, const QOpcUaMonitoringParameters &settings);
    bool disableMonitoring(QOpcUaNode::NodeAttributes attr);
    bool modifyMonitoring(QOpcUaNode::NodeAttribute attr, QOpcUaMonitoringParameters::Parameter item, const QVariant &value);
    QOpcUaMonitoringParameters monitoringStatus(QOpcUaNode::NodeAttribute attr);

    QStringList childrenIds() const;
    QString nodeId() const;

    QPair<double, double> readEuRange() const;
    QPair<QString, QString> readEui() const;

    bool callMethod(const QString &methodNodeId, const QVector<QOpcUa::TypedVariant> &args = QVector<QOpcUa::TypedVariant>());

Q_SIGNALS:
    void readFinished(QOpcUaNode::NodeAttributes attributes);
    void attributeWritten(QOpcUaNode::NodeAttribute attribute, QOpcUa::UaStatusCode statusCode);
    void attributeUpdated(QOpcUaNode::NodeAttribute attr, QVariant value);

    void monitoringStatusChanged(QOpcUaNode::NodeAttribute attr, QOpcUaMonitoringParameters::Parameters items,
                           QOpcUa::UaStatusCode statusCode);
    void enableMonitoringFinished(QOpcUaNode::NodeAttribute attr, QOpcUa::UaStatusCode statusCode);
    void disableMonitoringFinished(QOpcUaNode::NodeAttribute attr, QOpcUa::UaStatusCode statusCode);
    void methodCallFinished(QString methodNodeId, QVariant result, QOpcUa::UaStatusCode statusCode);

private:
    Q_DISABLE_COPY(QOpcUaNode)
};

Q_OPCUA_EXPORT QDebug operator<<(QDebug dbg, const QOpcUaNode &node);

Q_DECLARE_TYPEINFO(QOpcUaNode::NodeClass, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QOpcUaNode::NodeAttribute, Q_PRIMITIVE_TYPE);
Q_DECLARE_OPERATORS_FOR_FLAGS(QOpcUaNode::NodeAttributes)

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaNode::NodeClass)
Q_DECLARE_METATYPE(QOpcUaNode::NodeAttribute)
Q_DECLARE_METATYPE(QOpcUaNode::NodeAttributes)
Q_DECLARE_METATYPE(QOpcUaNode::AttributeMap)

inline Q_DECL_CONSTEXPR QOpcUaNode::NodeAttributes QOpcUaNode::mandatoryBaseAttributes()
{
    return QOpcUaNode::NodeAttribute::NodeId | QOpcUaNode::NodeAttribute::NodeClass |
            QOpcUaNode::NodeAttribute::BrowseName | QOpcUaNode::NodeAttribute::DisplayName;
}

inline Q_DECL_CONSTEXPR QOpcUaNode::NodeAttributes QOpcUaNode::allBaseAttributes()
{
    return NodeAttribute::NodeId | NodeAttribute::NodeClass | NodeAttribute::BrowseName |
            NodeAttribute::DisplayName | NodeAttribute::Description | NodeAttribute::WriteMask |
            NodeAttribute::UserWriteMask;
}

#endif // QOPCUANODE_H
