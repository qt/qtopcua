// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#define QT_OPCUA_BUILD_REMOVED_API

#include "qopcuaglobal.h"

QT_USE_NAMESPACE

#if QT_OPCUA_REMOVED_SINCE(6, 7)

#include "qopcuacontentfilterelement.h"

bool QOpcUaContentFilterElement::operator==(const QOpcUaContentFilterElement &rhs) const
{ return comparesEqual(*this, rhs); }

#include "qopcuarelativepathelement.h"

bool QOpcUaRelativePathElement::operator==(const QOpcUaRelativePathElement &rhs) const
{ return comparesEqual(*this, rhs); }

#include "qopcuasimpleattributeoperand.h"

bool QOpcUaSimpleAttributeOperand::operator==(const QOpcUaSimpleAttributeOperand &rhs) const
{ return comparesEqual(*this, rhs); }

// #include "qotherheader.h"
// // implement removed functions from qotherheader.h
// order sections alphabetically to reduce chances of merge conflicts

#endif // QT_OPCUA_REMOVED_SINCE(6, 7)
