/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
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

#pragma once

#include <QObject>
#include <qopcuatype.h>
#include "universalnode.h"

QT_BEGIN_NAMESPACE

class QOpcUaClient;
class QOpcUaRelativePathElement;

class OpcUaRelativeNodePath : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString ns READ nodeNamespace WRITE setNodeNamespace NOTIFY nodeNamespaceChanged)
    Q_PROPERTY(QString browseName READ browseName WRITE setBrowseName NOTIFY browseNameChanged)
    Q_PROPERTY(QVariant referenceType READ referenceType WRITE setReferenceType NOTIFY referenceTypeChanged)
    Q_PROPERTY(bool includeSubtypes READ includeSubtypes WRITE setIncludeSubtypes NOTIFY includeSubtypesChanged)
    Q_PROPERTY(bool isInverse READ isInverse WRITE setIsInverse NOTIFY isInverseChanged)

public:
    explicit OpcUaRelativeNodePath(QObject *parent = nullptr);
    const QString &nodeNamespace() const;
    const QString &browseName() const;
    QVariant referenceType() const;
    bool includeSubtypes() const;
    bool isInverse() const;
    QOpcUaRelativePathElement toRelativePathElement(QOpcUaClient *client) const;

signals:
    void nodeNamespaceChanged(QString ns);
    void browseNameChanged(QString browseName);
    void referenceTypeChanged();
    void includeSubtypesChanged(bool includeSubtypes);
    void isInverseChanged(bool isInverse);

public slots:
    void setNodeNamespace(QString ns);
    void setBrowseName(QString browseName);
    void setReferenceType(const QVariant &referenceType);
    void setIncludeSubtypes(bool includeSubtypes);
    void setIsInverse(bool isInverse);

private:
    mutable UniversalNode m_browseNode;
    QVariant m_referenceType = QVariant::fromValue(QOpcUa::ReferenceTypeId::References);
    bool m_includeSubtypes = true;
    bool m_isInverse = false;
};

QT_END_NAMESPACE
