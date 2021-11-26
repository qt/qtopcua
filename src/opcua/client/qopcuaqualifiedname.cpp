/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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

QDebug operator<<(QDebug debug, const QOpcUaQualifiedName &qn)
{
    QDebugStateSaver saver(debug);
    debug.nospace().quote() << "QOpcUaQualifiedname(" << qn.namespaceIndex() << ", " << qn.name() << ")";
    return debug;
}

QT_END_NAMESPACE
