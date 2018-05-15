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
#include "universalnode.h"

QT_BEGIN_NAMESPACE

class OpcUaNodeIdType : public QObject
{
    Q_OBJECT

public:
    /* These functions are not exposed to QML intentionally.
       The reason is that some derived QML types have to prohibit access to some of these functions.
       Exposing is done selectively in the derived classes.
    */
    explicit OpcUaNodeIdType(QObject *parent = nullptr);
    void setNodeNamespace(const QString &);
    const QString &nodeNamespace() const;
    void setIdentifier(const QString &);
    const QString &identifier() const;
    QString fullNodePath() const;
    void from(const OpcUaNodeIdType &);
    void from(const UniversalNode &);

signals:
    void nodeNamespaceChanged(const QString &);
    void identifierChanged(const QString &);
    void nodeChanged();

private:
    UniversalNode m_universalNode;

    friend class UniversalNode;
};

QT_END_NAMESPACE
