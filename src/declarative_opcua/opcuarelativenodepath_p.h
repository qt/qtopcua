// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUA_OPCUARELATIVENODEPATH_P_H
#define QOPCUA_OPCUARELATIVENODEPATH_P_H

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

#endif // QOPCUA_OPCUARELATIVENODEPATH_P_H
