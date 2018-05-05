/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt OPC UA module.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "treeitem.h"
#include "opcuamodel.h"
#include "common.h"
#include <QOpcUaClient>
#include <QMetaEnum>
#include <QPixmap>

QT_BEGIN_NAMESPACE

const int numberOfDisplayColumns = 7; // NodeId, Value, NodeClass, DataType, BrowseName, DisplayName, Description

TreeItem::TreeItem(OpcUaModel *model) : QObject(nullptr)
  , mModel(model)
{
}

TreeItem::TreeItem(QOpcUaNode *node, OpcUaModel *model, TreeItem *parent) : QObject(parent)
  , mOpcNode(node)
  , mModel(model)
  , mParentItem(parent)
{
    connect(mOpcNode.get(), &QOpcUaNode::attributeRead, this, &TreeItem::handleAttributes);
    connect(mOpcNode.get(), &QOpcUaNode::browseFinished, this, &TreeItem::browseFinished);

    if (!mOpcNode->readAttributes( QOpcUa::NodeAttribute::Value
                            | QOpcUa::NodeAttribute::NodeClass
                            | QOpcUa::NodeAttribute::Description
                            | QOpcUa::NodeAttribute::DataType
                            | QOpcUa::NodeAttribute::BrowseName
                            | QOpcUa::NodeAttribute::DisplayName
                            ))
        qWarning() << "Reading attributes" << mOpcNode->nodeId() << "failed";
}

TreeItem::TreeItem(QOpcUaNode *node, OpcUaModel *model, const QOpcUaReferenceDescription &browsingData, TreeItem *parent) : TreeItem(node, model, parent)
{
    mNodeBrowseName = browsingData.browseName().name;
    mNodeClass = browsingData.nodeClass();
    mNodeId = browsingData.nodeId();
    mNodeDisplayName = browsingData.displayName().text;
}

TreeItem::~TreeItem()
{
    qDeleteAll(mChildItems);
}

TreeItem *TreeItem::child(int row)
{
    if (row >= mChildItems.size())
        qCritical() << "TreeItem in row" << row << "does not exist.";
    return mChildItems[row];
}

int TreeItem::childIndex(const TreeItem *child) const
{
    return mChildItems.indexOf(const_cast<TreeItem *>(child));
}

int TreeItem::childCount()
{
    startBrowsing();
    return mChildItems.size();
}

int TreeItem::columnCount() const
{
    return numberOfDisplayColumns;
}

QVariant TreeItem::data(int column)
{
    if (column == 0) {
        return mNodeBrowseName;
    } else if (column == 1) {
        if (!mAttributesReady)
            return tr("Loading ...");

        const auto type = mOpcNode->attribute(QOpcUa::NodeAttribute::DataType).toString();
        const auto value = mOpcNode->attribute(QOpcUa::NodeAttribute::Value);

        if (value.type() == QVariant::Type::List)
            return value.toStringList().join("\n");
        else if (type == "ns=0;i=3")
            return QLatin1String("0x") + QString::number(value.toUInt(), 16);
        else
            return mOpcNode->attribute(QOpcUa::NodeAttribute::Value);
    } else if (column == 2) {
        QMetaEnum metaEnum = QMetaEnum::fromType<QOpcUa::NodeClass>();
        QString name = metaEnum.valueToKey((uint)mNodeClass);
        return name + " (" + QString::number((uint)mNodeClass) + ")";
    } else if (column == 3) {
        if (!mAttributesReady)
            return tr("Loading ...");

        const auto dti = dataTypeInfo(mOpcNode->attribute(QOpcUa::NodeAttribute::DataType).toString());
        if (dti.name.isEmpty())
            return QString();
        return dti.name + " (" + mOpcNode->attribute(QOpcUa::NodeAttribute::DataType).toString() + ")";
    } else if (column == 4) {
        return mNodeId;
    } else if (column == 5) {
        return mNodeDisplayName;
    } else if (column == 6) {
        if (!mAttributesReady)
            return tr("Loading ...");

        return mOpcNode->attribute(QOpcUa::NodeAttribute::Description);
    }
    return QVariant();
}

int TreeItem::row() const
{
    if (!mParentItem)
        return 0;
    return mParentItem->childIndex(this);
}

TreeItem *TreeItem::parentItem()
{
    return mParentItem;
}

void TreeItem::appendChild(TreeItem *child)
{
    mChildItems.append(child);
}

QPixmap TreeItem::icon(int column) const
{
    if (column != 0 || !mOpcNode)
        return QPixmap();

    QColor c;

    switch (mNodeClass) {
    case QOpcUa::NodeClass::Object:
        c = Qt::darkGreen;
        break;
    case QOpcUa::NodeClass::Variable:
        c = Qt::darkBlue;
        break;
    case QOpcUa::NodeClass::Method:
        c = Qt::darkRed;
        break;
    default:
        c = Qt::gray;
    }

    QPixmap p(10,10);
    p.fill(c);
    return p;
}

void TreeItem::startBrowsing()
{
    if (mBrowseStarted)
        return;

    if (!mOpcNode->browseChildren())
        qWarning() << "Browsing node" << mOpcNode->nodeId() << "failed";
    else
        mBrowseStarted = true;
}

void TreeItem::handleAttributes(QOpcUa::NodeAttributes attr)
{
    if (attr & QOpcUa::NodeAttribute::NodeClass)
        mNodeClass = mOpcNode->attribute(QOpcUa::NodeAttribute::NodeClass).value<QOpcUa::NodeClass>();
    if (attr & QOpcUa::NodeAttribute::BrowseName)
        mNodeBrowseName = mOpcNode->attribute(QOpcUa::NodeAttribute::BrowseName).value<QOpcUa::QQualifiedName>().name;
    if (attr & QOpcUa::NodeAttribute::DisplayName)
        mNodeDisplayName = mOpcNode->attribute(QOpcUa::NodeAttribute::DisplayName).value<QOpcUa::QLocalizedText>().text;
    if (attr & QOpcUa::NodeAttribute::NodeId)
        mNodeDisplayName = mOpcNode->attribute(QOpcUa::NodeAttribute::NodeId).toString();

    mAttributesReady = true;
    emit mModel->dataChanged(mModel->createIndex(row(), 0, this), mModel->createIndex(row(), numberOfDisplayColumns - 1, this));
}

void TreeItem::browseFinished(QVector<QOpcUaReferenceDescription> children, QOpcUa::UaStatusCode statusCode)
{
    if (statusCode != QOpcUa::Good) {
        qWarning() << "Browsing node" << mOpcNode->nodeId() << "finally failed:" << statusCode;
        return;
    }

    auto index = mModel->createIndex(row(), 0, this);

    for (const auto &item : children) {
        mModel->beginInsertRows(index, mChildItems.size(), mChildItems.size() + children.size());
        auto node = mModel->opcUaClient()->node(item.nodeId());
        if (!node) {
            qWarning() << "Failed to instantiate node:" << item.nodeId();
            continue;
        }

        mChildItems.append(new TreeItem(node, mModel, item, this));
        mModel->endInsertRows();
    }

    emit mModel->dataChanged(mModel->createIndex(row(), 0, this), mModel->createIndex(row(), numberOfDisplayColumns - 1, this));
}

QT_END_NAMESPACE
