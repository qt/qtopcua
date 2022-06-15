// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUARELATIVEPATHELEMENT_H
#define QOPCUARELATIVEPATHELEMENT_H

#include <QtOpcUa/qopcuatype.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOpcUaQualifiedName;

class QOpcUaRelativePathElementData;
class Q_OPCUA_EXPORT QOpcUaRelativePathElement
{
public:
    QOpcUaRelativePathElement();
    QOpcUaRelativePathElement(const QOpcUaQualifiedName &target, const QString &refType);
    QOpcUaRelativePathElement(const QOpcUaQualifiedName &target, QOpcUa::ReferenceTypeId refType);
    QOpcUaRelativePathElement(const QOpcUaRelativePathElement &);
    QOpcUaRelativePathElement &operator=(const QOpcUaRelativePathElement &);
    bool operator==(const QOpcUaRelativePathElement &rhs) const;
    ~QOpcUaRelativePathElement();

    QString referenceTypeId() const;
    void setReferenceTypeId(const QString &referenceTypeId);
    void setReferenceTypeId(QOpcUa::ReferenceTypeId referenceTypeId);

    bool isInverse() const;
    void setIsInverse(bool isInverse);

    bool includeSubtypes() const;
    void setIncludeSubtypes(bool includeSubtypes);

    QOpcUaQualifiedName targetName() const;
    void setTargetName(const QOpcUaQualifiedName &targetName);

private:
    QSharedDataPointer<QOpcUaRelativePathElementData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaRelativePathElement)

#endif // QOPCUARELATIVEPATHELEMENT_H
