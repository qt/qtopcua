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

#include "qopcuatype.h"

QT_BEGIN_NAMESPACE

/*!
    \namespace QOpcUa
    \ingroup modules
    \inmodule QtOpcua

    \brief The QOpcUa namespace contains miscellaneous identifiers used throughout the Qt OpcUa library.
*/

/*!
    \enum QOpcUa::Types

    Enumerates the types supported by Qt OpcUa.

    \value Boolean
    \value Int32
    \value UInt32
    \value Double
    \value Float
    \value String
    \value LocalizedText
    \value DateTime
    \value UInt16
    \value Int16
    \value UInt64
    \value Int64
    \value Byte
    \value SByte
    \value ByteString
    \value XmlElement \warning Currently not supported by the FreeOPCUA backend
    \value NodeId \warning Currently not supported by the FreeOPCUA backend
    \value Guid
    \value QualifiedName
    \value Undefined
*/

/*!
    \typedef QOpcUa::TypedVariant

    This is QPair<QVariant, QOpcUa::Types>.
*/

/*!
  \class QOpcUa::QQualifiedName

  This is the QtOpcUa representation for the OPC UA QualifiedName type defined in OPC-UA part 3, 8.3.
  A QualifiedName is a name qualified by a namespace index. The namespace index corresponds to an entry in the server's namespace array.
  QualifiedName is mainly used to represent the BrowseName attribute of a node.
*/

/*!
    \fn QOpcUa::QQualifiedName::QQualifiedName(quint16 idx, QString text)

    Constructs an instance of QQualifiedName with the namespace index given in \a idx and the name given in \a text.
*/

/*!
    \variable QOpcUa::QQualifiedName::namespaceIndex

    The namespace index of the QQualifiedName.
*/

/*!
    \variable QOpcUa::QQualifiedName::name

    The name string of the QQualifiedName.
*/


QT_END_NAMESPACE
