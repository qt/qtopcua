/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
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

#pragma once

#include <QObject>
#include <qopcuatype.h>
#include "universalnode.h"

QT_BEGIN_NAMESPACE

class QOpcUaClient;

class OpcUaRelativeNodePath : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString ns READ nodeNamespace WRITE setNodeNamespace NOTIFY nodeNamespaceChanged)
    Q_PROPERTY(QString browseName READ browseName WRITE setBrowseName NOTIFY browseNameChanged)
    Q_PROPERTY(QOpcUa::ReferenceTypeId referenceType READ referenceType WRITE setReferenceType NOTIFY referenceTypeChanged)
    Q_PROPERTY(bool includeSubtypes READ includeSubtypes WRITE setIncludeSubtypes NOTIFY includeSubtypesChanged)
    Q_PROPERTY(bool isInverse READ isInverse WRITE setIsInverse NOTIFY isInverseChanged)

public:
    explicit OpcUaRelativeNodePath(QObject *parent = nullptr);
    const QString &nodeNamespace() const;
    const QString &browseName() const;
    QOpcUa::ReferenceTypeId referenceType() const;
    bool includeSubtypes() const;
    bool isInverse() const;
    QOpcUa::QRelativePathElement toRelativePathElement(QOpcUaClient *client) const;

signals:
    void nodeNamespaceChanged(QString ns);
    void browseNameChanged(QString browseName);
    void referenceTypeChanged(QOpcUa::ReferenceTypeId referenceType);
    void includeSubtypesChanged(bool includeSubtypes);
    void isInverseChanged(bool isInverse);

public slots:
    void setNodeNamespace(QString ns);
    void setBrowseName(QString browseName);
    void setReferenceType(QOpcUa::ReferenceTypeId referenceType);
    void setIncludeSubtypes(bool includeSubtypes);
    void setIsInverse(bool isInverse);

private:
    mutable UniversalNode m_browseNode;
    QOpcUa::ReferenceTypeId m_referenceType = QOpcUa::ReferenceTypeId::References;
    bool m_includeSubtypes = true;
    bool m_isInverse = false;
};

QT_END_NAMESPACE
