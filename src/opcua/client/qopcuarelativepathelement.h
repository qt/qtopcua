/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

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
