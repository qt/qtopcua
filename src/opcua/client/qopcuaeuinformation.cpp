// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcuaeuinformation.h"
#include "qopcualocalizedtext.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaEUInformation
    \inmodule QtOpcUa
    \brief The OPC UA EURange type.

    This is the Qt OPC UA representation for the OPC UA EUInformation type defined in OPC UA 1.05 part 8, 5.6.3.
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

/*!
    Default constructs an EU information with no parameters set.
*/
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
