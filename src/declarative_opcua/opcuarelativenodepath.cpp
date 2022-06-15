// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <private/opcuarelativenodepath_p.h>
#include <private/opcuanodeid_p.h>

#include <QtOpcUa/qopcuaclient.h>
#include <QLoggingCategory>
#include <QMetaEnum>

QT_BEGIN_NAMESPACE

/*!
    \qmltype RelativeNodePath
    \inqmlmodule QtOpcUa
    \brief Specifies a relative node path element.
    \since QtOpcUa 5.12

    \code
    import QtOpcUa as QtOpcUa

    QtOpcUa.RelativeNodePath {
        ns: "Test Namespace"
        browseName: "SomeName"
    }
    \endcode

    \sa Node, NodeId, RelativeNodeId
*/

/*!
    \qmlproperty NodeId RelativeNodePath::ns

    Namespace name of this path element.
    The identifier can be the index as a number or the name as string.
    A string which can be converted to an integer is considered a namespace index.
*/

/*!
    \qmlproperty NodeId RelativeNodePath::browseName

    Browse name of this path element.
*/

/*!
    \qmlproperty bool RelativeNodePath::includeSubtypes

    Whether subtypes are included when matching this path element.
    The default value of this property is \c true.
*/

/*!
    \qmlproperty bool RelativeNodePath::isInverse

    Whether the reference to follow is inverse.
    The default value of this property is \c false.
*/

/*!
    \qmlproperty QOpcUa::ReferenceTypeId RelativeNodePath::referenceType

    Type of reference when matching this path element.
    This can be a \l QOpcUa::ReferenceTypeId or a \l NodeId.
    The default value of this property is \c Constants.ReferenceTypeId.References.
*/

Q_DECLARE_LOGGING_CATEGORY(QT_OPCUA_PLUGINS_QML)

OpcUaRelativeNodePath::OpcUaRelativeNodePath(QObject *parent) : QObject(parent)
{
    connect(&m_browseNode, &UniversalNode::namespaceNameChanged, this, &OpcUaRelativeNodePath::nodeNamespaceChanged);
    connect(&m_browseNode, &UniversalNode::nodeIdentifierChanged, this, &OpcUaRelativeNodePath::browseNameChanged);
}

const QString &OpcUaRelativeNodePath::nodeNamespace() const
{
    return m_browseNode.namespaceName();
}

const QString &OpcUaRelativeNodePath::browseName() const
{
    return m_browseNode.nodeIdentifier();
}

QVariant OpcUaRelativeNodePath::referenceType() const
{
    return m_referenceType;
}

bool OpcUaRelativeNodePath::includeSubtypes() const
{
    return m_includeSubtypes;
}

void OpcUaRelativeNodePath::setNodeNamespace(QString ns)
{
    m_browseNode.setNamespace(ns);
}

void OpcUaRelativeNodePath::setBrowseName(QString browseName)
{
    m_browseNode.setNodeIdentifier(browseName);
}

void OpcUaRelativeNodePath::setReferenceType(const QVariant &referenceType)
{
    bool valid = false;

    if (referenceType.userType() == qMetaTypeId<QObject*>() && qobject_cast<OpcUaNodeId*>(referenceType.value<QObject*>()))
        valid = true;
    else if (referenceType.userType() == QMetaType::Int && QMetaEnum::fromType<QOpcUa::ReferenceTypeId>().valueToKey(referenceType.toInt()))
        valid = true;

    if (!valid) {
            qCWarning(QT_OPCUA_PLUGINS_QML) << "Invalid reference type:" << referenceType;
            return;
    }

    if (m_referenceType == referenceType)
        return;

    m_referenceType = referenceType;
    emit referenceTypeChanged();
}

void OpcUaRelativeNodePath::setIncludeSubtypes(bool includeSubtypes)
{
    if (m_includeSubtypes == includeSubtypes)
        return;

    m_includeSubtypes = includeSubtypes;
    emit includeSubtypesChanged(m_includeSubtypes);
}

bool OpcUaRelativeNodePath::isInverse() const
{
    return m_isInverse;
}

QOpcUaRelativePathElement OpcUaRelativeNodePath::toRelativePathElement(QOpcUaClient *client) const
{
    m_browseNode.resolveNamespaceNameToIndex(client);

    QOpcUaRelativePathElement x;
    x.setIsInverse(isInverse());
    x.setIncludeSubtypes(includeSubtypes());
    x.setTargetName(m_browseNode.toQualifiedName());
    if (m_referenceType.userType() == QMetaType::Int
        || m_referenceType.userType() == qMetaTypeId<QOpcUa::ReferenceTypeId>())
        x.setReferenceTypeId(m_referenceType.value<QOpcUa::ReferenceTypeId>());
    else
        x.setReferenceTypeId(m_referenceType.toString());

    return x;
}

void OpcUaRelativeNodePath::setIsInverse(bool isInverse)
{
    if (m_isInverse == isInverse)
        return;

    m_isInverse = isInverse;
    emit isInverseChanged(m_isInverse);
}

QT_END_NAMESPACE
