/****************************************************************************
**
** Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
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

#ifndef QOPCUABROWSING_H
#define QOPCUABROWSING_H

#include <QtOpcUa/qopcuatype.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOpcUaBrowseRequestData;
class Q_OPCUA_EXPORT QOpcUaBrowseRequest
{
public:

    enum class BrowseDirection : quint32 {
        Forward = 0,
        Inverse = 1,
        Both = 2
    };

    QOpcUaBrowseRequest();
    QOpcUaBrowseRequest(const QOpcUaBrowseRequest &other);
    QOpcUaBrowseRequest &operator=(const QOpcUaBrowseRequest &rhs);
    ~QOpcUaBrowseRequest();

    QOpcUaBrowseRequest::BrowseDirection browseDirection() const;
    void setBrowseDirection(const QOpcUaBrowseRequest::BrowseDirection &browseDirection);

    QString referenceTypeId() const;
    void setReferenceTypeId(const QString &referenceTypeId);
    void setReferenceTypeId(QOpcUa::ReferenceTypeId referenceTypeId);

    bool includeSubtypes() const;
    void setIncludeSubtypes(bool includeSubtypes);

    QOpcUa::NodeClasses nodeClassMask() const;
    void setNodeClassMask(const QOpcUa::NodeClasses &nodeClassMask);

private:
    QSharedDataPointer<QOpcUaBrowseRequestData> data;
};

class QOpcUaReferenceDescriptionPrivate;

class Q_OPCUA_EXPORT QOpcUaReferenceDescription
{
public:
    QOpcUaReferenceDescription();
    QOpcUaReferenceDescription(const QOpcUaReferenceDescription &other);
    QOpcUaReferenceDescription &operator=(const QOpcUaReferenceDescription &other);

    ~QOpcUaReferenceDescription();

    QString refTypeId() const;
    void setRefTypeId(const QString &refTypeId);
    QOpcUa::QExpandedNodeId targetNodeId() const;
    void setTargetNodeId(const QOpcUa::QExpandedNodeId &targetNodeId);
    QOpcUa::QQualifiedName browseName() const;
    void setBrowseName(const QOpcUa::QQualifiedName &browseName);
    QOpcUa::QLocalizedText displayName() const;
    void setDisplayName(const QOpcUa::QLocalizedText &displayName);
    QOpcUa::NodeClass nodeClass() const;
    void setNodeClass(QOpcUa::NodeClass nodeClass);
    void setIsForward(bool isForward);
    bool isForward() const;
    void setTypeDefinition(const QOpcUa::QExpandedNodeId &typeDefinition);
    QOpcUa::QExpandedNodeId typeDefinition() const;

private:
    QSharedDataPointer<QOpcUaReferenceDescriptionPrivate> d_ptr;
};

Q_DECLARE_TYPEINFO(QOpcUaReferenceDescription, Q_MOVABLE_TYPE);

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaReferenceDescription)
Q_DECLARE_METATYPE(QOpcUaBrowseRequest)

#endif // QOPCUABROWSING_H
