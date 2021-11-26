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

#include <private/opcuanodeidtype_p.h>

#include <QList>
#include <QQmlListProperty>
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

class OpcUaRelativeNodePath;
class OpcUaNodeIdType;

class OpcUaRelativeNodeId : public OpcUaNodeIdType
{
    Q_OBJECT
    Q_DISABLE_COPY(OpcUaRelativeNodeId)
    Q_PROPERTY(OpcUaNodeIdType* startNode READ startNode WRITE setStartNode NOTIFY startNodeChanged)
    Q_PROPERTY(QQmlListProperty<OpcUaRelativeNodePath> path READ paths)

    QML_NAMED_ELEMENT(RelativeNodeId)
    QML_ADDED_IN_VERSION(5, 12)

public:
    explicit OpcUaRelativeNodeId(QObject *parent = nullptr);

    OpcUaNodeIdType *startNode() const;
    QQmlListProperty<OpcUaRelativeNodePath> paths();
    void appendPath(OpcUaRelativeNodePath *path);
    int pathCount() const;
    OpcUaRelativeNodePath *path(int) const;
    void clearPaths();

signals:
    void startNodeChanged(OpcUaNodeIdType *startNode);
    void pathChanged();

public slots:
    void setStartNode(OpcUaNodeIdType *startNode);

private:
    static void appendPath(QQmlListProperty<OpcUaRelativeNodePath>*, OpcUaRelativeNodePath *);
    static qsizetype pathCount(QQmlListProperty<OpcUaRelativeNodePath>*);
    static OpcUaRelativeNodePath* path(QQmlListProperty<OpcUaRelativeNodePath>*, qsizetype);
    static void clearPaths(QQmlListProperty<OpcUaRelativeNodePath>*);

    OpcUaNodeIdType *m_startNode = nullptr;
    QList<OpcUaRelativeNodePath *> m_paths;

};

QT_END_NAMESPACE
