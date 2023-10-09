// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuaqualifiedname.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaQualifiedName
    \inmodule QtOpcUa
    \brief The OPC UA QualifiedName type.

    This is the Qt OPC UA representation for the OPC UA QualifiedName type defined in OPC-UA part 3, 8.3.
    A QualifiedName is a name qualified by a namespace index. The namespace index corresponds to an entry in the server's namespace array.
    QualifiedName is mainly used to represent the BrowseName attribute of a node.
*/

class QOpcUaQualifiedNameData : public QSharedData
{
public:
    QString name;
    quint16 namespaceIndex{0}; //OPC UA part 4, page 116: a string is converted to a qualified name by setting the namespace index to 0.
};

/*!
    Default constructs a qualified name with no parameters set.
*/
QOpcUaQualifiedName::QOpcUaQualifiedName()
    : data(new QOpcUaQualifiedNameData)
{
}

/*!
    Constructs a qualified name from \a rhs.
*/
QOpcUaQualifiedName::QOpcUaQualifiedName(const QOpcUaQualifiedName &rhs)
    : data(rhs.data)
{
}

/*!
    Constructs a qualified name with namespace index \a namespaceIndex and the name \a name.
*/
QOpcUaQualifiedName::QOpcUaQualifiedName(quint16 namespaceIndex, const QString &name)
    : data(new QOpcUaQualifiedNameData)
{
    data->namespaceIndex = namespaceIndex;
    data->name = name;
}

/*!
    Returns \c true if this qualified name has the same value as \a rhs.
*/
bool QOpcUaQualifiedName::operator==(const QOpcUaQualifiedName &rhs) const
{
    return data->namespaceIndex == rhs.namespaceIndex() &&
            data->name == rhs.name();
}

/*!
    Converts this qualified name to \l QVariant.
*/
QOpcUaQualifiedName::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

/*!
    Sets the values from \a rhs in this qualified name.
*/
QOpcUaQualifiedName &QOpcUaQualifiedName::operator=(const QOpcUaQualifiedName &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

QOpcUaQualifiedName::~QOpcUaQualifiedName()
{
}

/*!
    Returns the namespace index.
*/
quint16 QOpcUaQualifiedName::namespaceIndex() const
{
    return data->namespaceIndex;
}

/*!
    Sets the namespace index to \a namespaceIndex.
*/
void QOpcUaQualifiedName::setNamespaceIndex(quint16 namespaceIndex)
{
    data->namespaceIndex = namespaceIndex;
}

/*!
    Returns the name.
*/
QString QOpcUaQualifiedName::name() const
{
    return data->name;
}

/*!
    Sets the name to \a name.
*/
void QOpcUaQualifiedName::setName(const QString &name)
{
    data->name = name;
}

#ifndef QT_NO_DEBUG_STREAM

/*!
    \fn QDebug QOpcUaQualifiedName::operator<<(QDebug debug, const QOpcUaQualifiedName &name)
    \since 6.3

    Writes the qualified \a name to the \a debug output.

    \sa QDebug
*/
QDebug operator<<(QDebug debug, const QOpcUaQualifiedName &qn)
{
    QDebugStateSaver saver(debug);
    debug.nospace().quote() << "QOpcUaQualifiedname(" << qn.namespaceIndex() << ", " << qn.name() << ")";
    return debug;
}

#endif

QT_END_NAMESPACE
