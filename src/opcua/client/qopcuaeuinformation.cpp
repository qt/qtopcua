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

#include "qopcuaeuinformation.h"
#include "qopcualocalizedtext.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaEUInformation
    \inmodule QtOpcUa
    \brief The OPC UA EURange type.

    This is the Qt OPC UA representation for the OPC UA EUInformation type defined in OPC-UA part 8, 5.6.3.
    EUInformation values contain information about units and are mostly used as property of a node with a numeric value attribute.
    The information can e. g. be used to add text and tooltips to GUI elements.
*/

class QOpcUaEUInformationData : public QSharedData
{
public:
    QString namespaceUri;
    qint32 unitId{0};
    QOpcUaLocalizedText displayName;
    QOpcUaLocalizedText description;
};

QOpcUaEUInformation::QOpcUaEUInformation()
    : data(new QOpcUaEUInformationData)
{
}

/*!
    Constructs a EUinformation from \a rhs.
*/
QOpcUaEUInformation::QOpcUaEUInformation(const QOpcUaEUInformation &rhs)
    : data(rhs.data)
{
}

/*!
    Constructs a EUinformation with namespace URI \a namespaceUri, unit id \a unitId,
    display name \a displayName and description \a description.
*/
QOpcUaEUInformation::QOpcUaEUInformation(const QString &namespaceUri, qint32 unitId, const QOpcUaLocalizedText &displayName,
                                       const QOpcUaLocalizedText &description)
    : data(new QOpcUaEUInformationData)
{
    data->namespaceUri = namespaceUri;
    data->unitId = unitId;
    data->displayName = displayName;
    data->description = description;
}


/*!
    Sets the values from \a rhs in this EUinformation.
*/
QOpcUaEUInformation &QOpcUaEUInformation::operator=(const QOpcUaEUInformation &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Returns \c true if this EUinformation has the same value as \a rhs.
*/
bool QOpcUaEUInformation::operator==(const QOpcUaEUInformation &rhs) const
{
    return data->namespaceUri == rhs.namespaceUri() &&
            data->unitId == rhs.unitId() &&
            data->displayName == rhs.displayName() &&
            data->description == rhs.description();
}

QOpcUaEUInformation::~QOpcUaEUInformation()
{
}

/*!
    Returns the description of the unit, for example \e {degree Celsius}.
*/
QOpcUaLocalizedText QOpcUaEUInformation::description() const
{
    return data->description;
}

/*!
    Sets the description if the unit to \a description.
*/
void QOpcUaEUInformation::setDescription(const QOpcUaLocalizedText &description)
{
    data->description = description;
}

/*!
    Returns the display name of the unit, for example \e {°C}.
*/
QOpcUaLocalizedText QOpcUaEUInformation::displayName() const
{
    return data->displayName;
}

/*!
    Sets the display name of the unit to \a displayName.
*/
void QOpcUaEUInformation::setDisplayName(const QOpcUaLocalizedText &displayName)
{
    data->displayName = displayName;
}

/*!
    Returns the machine-readable identifier for the unit.
*/
qint32 QOpcUaEUInformation::unitId() const
{
    return data->unitId;
}

/*!
    Sets the machine-readable identifier for the unit to \a unitId.
*/
void QOpcUaEUInformation::setUnitId(qint32 unitId)
{
    data->unitId = unitId;
}

/*!
    Returns the namespace URI of the unit.
*/
QString QOpcUaEUInformation::namespaceUri() const
{
    return data->namespaceUri;
}

/*!
    Sets the namespace URI of the unit to \a namespaceUri.
*/
void QOpcUaEUInformation::setNamespaceUri(const QString &namespaceUri)
{
    data->namespaceUri = namespaceUri;
}

/*!
    Converts this EUinformation to \l QVariant.
*/
QOpcUaEUInformation::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

QT_END_NAMESPACE
