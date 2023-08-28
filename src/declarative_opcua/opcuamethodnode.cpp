// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <private/opcuamethodnode_p.h>
#include <private/opcuanodeidtype_p.h>

#include <QOpcUaNode>
#include <QLoggingCategory>

QT_BEGIN_NAMESPACE

/*!
    \qmltype MethodNode
    \inqmlmodule QtOpcUa
    \brief Calls a method on the server.
    \since QtOpcUa 5.12
    \inherits Node

    This QML element supports calling method nodes on a server.
    The target object node ID has to be specified by the \l objectNodeId property.

    \code
    import QtOpcUa as QtOpcUa

    QtOpcUa.MethodNode {
        nodeId : QtOpcUa.NodeId {
            identifier: "s=Example.Method"
            ns: "Example Namespace"
        }
        objectNodeId : QtOpcUa.NodeId {
            identifier: "s=Example.Object"
            ns: "Example Namespace"
        }
        connection: myConnection
     }
     \endcode

     The actual function call can be triggered by a signal.

     \code
     Button {
           text: "Start"
           onClicked: myNode.callMethod
     }
     \endcode

     or by JavaScript

     \code
          myNode.callMethod()
     \endcode
*/

/*!
    \qmlmethod MethodNode::callMethod

    Calls the method on the connected server.
*/

/*!
    \qmlproperty OpcUaNode MethodNode::objectNodeId

    Determines the actual node on which the method is called.
    It can be a relative or absolute node Id.
*/

/*!
    \qmlproperty Status MethodNode::resultStatus
    \readonly

    Status of the last method call. This property has to be checked
    to determine if the method call was successful.

    \sa Status
*/

/*!
    \qmlproperty list<MethodArgument> MethodNode::inputArguments

    Arguments to be used when calling the method on the server.

    This example shows how to call a method with two double arguments.
    \code
    QtOpcUa.MethodNode {
        ...
        inputArguments: [
            QtOpcUa.MethodArgument {
                value: 3
                type: QtOpcUa.Constants.Double
            },
            QtOpcUa.MethodArgument {
                value: 4
                type: QtOpcUa.Constants.Double
            }
        ]
    }
    \endcode

    \sa callMethod
*/

/*!
    \qmlproperty list<var> MethodNode::outputArguments
    \readonly

    Returns values from the method call. Depending on the output arguments,
    this list may contain zero or more values. The \l resultStatus has to be checked
    separately. In case the method call failed, the list will be empty.

    \code
    if (node.status.isGood) {
        // print two arguments
        console.log("Number of return values:", node.outputArguments.length)
        console.log("Return value #1:", node.outputArguments[0])
        console.log("Return value #2:", node.outputArguments[1])
    }
    \endcode

    \sa callMethod, resultStatus
*/

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_QML)

OpcUaMethodNode::OpcUaMethodNode(QObject *parent):
    OpcUaNode(parent)
{
}

OpcUaNodeIdType *OpcUaMethodNode::objectNodeId() const
{
    return m_objectNodeId;
}

QQmlListProperty<OpcUaMethodArgument> OpcUaMethodNode::inputArguments()
{
    return QQmlListProperty<OpcUaMethodArgument>(this,
        &m_inputArguments,
        &OpcUaMethodNode::appendArgument,
        &OpcUaMethodNode::argumentCount,
        &OpcUaMethodNode::argument,
        &OpcUaMethodNode::clearArguments);
}

QVariantList OpcUaMethodNode::outputArguments()
{
    return m_outputArguments;
}

void OpcUaMethodNode::setObjectNodeId(OpcUaNodeIdType *node)
{
    if (m_objectNodeId)
        disconnect(m_objectNodeId);

    m_objectNodeId = node;
    connect(m_objectNodeId, &OpcUaNodeIdType::nodeChanged, this, &OpcUaMethodNode::handleObjectNodeIdChanged);
    handleObjectNodeIdChanged();
}

void OpcUaMethodNode::callMethod()
{
    if (!m_objectNode) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << "No object node";
        setStatus(Status::InvalidObjectNode);
        return;
    }
    if (!m_objectNode->node()) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << "Invalid object node";
        setStatus(Status::InvalidObjectNode);
        return;
    }
    if (!m_node) {
        qCWarning(QT_OPCUA_PLUGINS_QML) << "Invalid node Id";
        setStatus(Status::InvalidNodeId);
        return;
    }

    QList<QOpcUa::TypedVariant> arguments;
    for (const auto item : std::as_const(m_inputArguments))
        arguments.push_back(QOpcUa::TypedVariant(item->value(), item->type()));
    m_objectNode->node()->callMethod(m_node->nodeId(), arguments);
}

void OpcUaMethodNode::handleObjectNodeIdChanged()
{
    m_objectNode->deleteLater();
    m_objectNode = new OpcUaNode(this);
    m_objectNode->setNodeId(m_objectNodeId);
    connect(m_objectNode, &OpcUaNode::readyToUseChanged, this, [this](){
        connect(m_objectNode->node(), &QOpcUaNode::methodCallFinished, this, &OpcUaMethodNode::handleMethodCallFinished, Qt::UniqueConnection);
        checkValidity();
    });

    emit objectNodeIdChanged();
}

void OpcUaMethodNode::handleMethodCallFinished(QString methodNodeId, QVariant result, QOpcUa::UaStatusCode statusCode)
{
    Q_UNUSED(methodNodeId);
    m_resultStatus = OpcUaStatus(statusCode);

    m_outputArguments.clear();
    if (result.canConvert<QVariantList>())
        m_outputArguments = result.value<QVariantList>();
    else
        m_outputArguments.append(result);
    emit resultStatusChanged(m_resultStatus);
    emit outputArgumentsChanged();
}

void OpcUaMethodNode::setupNode(const QString &absolutePath)
{
    OpcUaNode::setupNode(absolutePath);
}

bool OpcUaMethodNode::checkValidity()
{
    if (m_node->attribute(QOpcUa::NodeAttribute::NodeClass).value<QOpcUa::NodeClass>() != QOpcUa::NodeClass::Method) {
        setStatus(Status::InvalidNodeType);
        return false;
    }
    if (!m_objectNode || !m_objectNode->node()) {
        setStatus(Status::InvalidObjectNode);
        return false;
    }

    const auto objectNodeClass = m_objectNode->node()->attribute(QOpcUa::NodeAttribute::NodeClass).value<QOpcUa::NodeClass>();
    if (objectNodeClass != QOpcUa::NodeClass::Object && objectNodeClass != QOpcUa::NodeClass::ObjectType) {
        setStatus(Status::InvalidObjectNode, tr("Object node is not of type `Object' or `ObjectType'"));
        return false;
    }

    setStatus(Status::Valid);

    return true;
}

OpcUaStatus OpcUaMethodNode::resultStatus() const
{
    return m_resultStatus;
}

void OpcUaMethodNode::appendArgument(QQmlListProperty<OpcUaMethodArgument>* list, OpcUaMethodArgument* p) {
    reinterpret_cast< QList<OpcUaMethodArgument*>* >(list->data)->append(p);
}

void OpcUaMethodNode::clearArguments(QQmlListProperty<OpcUaMethodArgument>* list) {
    reinterpret_cast< QList<OpcUaMethodArgument*>* >(list->data)->clear();
}

OpcUaMethodArgument* OpcUaMethodNode::argument(QQmlListProperty<OpcUaMethodArgument>* list, qsizetype i) {
    return reinterpret_cast< QList<OpcUaMethodArgument*>* >(list->data)->at(i);
}

qsizetype OpcUaMethodNode::argumentCount(QQmlListProperty<OpcUaMethodArgument>* list) {
    return reinterpret_cast< QList<OpcUaMethodArgument*>* >(list->data)->size();
}


QT_END_NAMESPACE
