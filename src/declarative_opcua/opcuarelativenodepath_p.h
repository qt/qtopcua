/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
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

#pragma once

#include <QObject>
#include <qopcuatype.h>

#include <private/universalnode_p.h>

#include <QtQml/qqml.h>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

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

    QML_NAMED_ELEMENT(RelativeNodePath)
    QML_ADDED_IN_VERSION(5, 12)

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
