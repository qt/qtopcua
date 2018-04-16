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

#include "opcuamodel.h"
#include "treeitem.h"
#include <QOpcUaClient>
#include <QOpcUaNode>
#include <QIcon>

QT_BEGIN_NAMESPACE

OpcUaModel::OpcUaModel(QObject *parent) : QAbstractItemModel(parent)
{
}

void OpcUaModel::setOpcUaClient(QOpcUaClient *client)
{
    beginResetModel();
    mOpcUaClient = client;
    if (mOpcUaClient)
        mRootItem.reset(new TreeItem(client->node("ns=0;i=84"), this /* model */, nullptr /* parent */));
    else
        mRootItem.reset(nullptr);
    endResetModel();
}

QOpcUaClient *OpcUaModel::opcUaClient() const
{
    return mOpcUaClient;
}

QVariant OpcUaModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto item = static_cast<TreeItem *>(index.internalPointer());

    if (role == Qt::DisplayRole) {
        return item->data(index.column());
    } else if (role ==  Qt::DecorationRole && index.column() == 0) {
        return item->icon(index.column());
    }

    return QVariant();
}

QVariant OpcUaModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        if (section == 0)
            return QString("BrowseName");
        else if (section == 1)
            return QString("Value");
        else if (section == 2)
            return QString("NodeClass");
        else if (section == 3)
            return QString("DataType");
        else if (section == 4)
            return QString("NodeId");
        else if (section == 5)
            return QString("DisplayName");
        else if (section == 6)
            return QString("Description");
        else
            return QString("Column %1").arg(section);
    }
    else
        return QString("Row %1").arg(section);
}

QModelIndex OpcUaModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *item = nullptr;

    if (!parent.isValid()) {
        item = mRootItem.get();
    } else {
        item = static_cast<TreeItem*>(parent.internalPointer())->child(row);
    }

    if (item)
        return createIndex(row, column, item);
    else
        return QModelIndex();
}

QModelIndex OpcUaModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    auto childItem = static_cast<TreeItem*>(index.internalPointer());
    auto parentItem = childItem->parentItem();

    if (childItem == mRootItem.get() || !parentItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int OpcUaModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (!mOpcUaClient)
        return 0;

    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        return 1; // only one root item
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    if (!parentItem)
        return 0;

    return parentItem->childCount();
}

int OpcUaModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else if (mRootItem)
        return mRootItem->columnCount();
    else
        return 0;
}

QT_END_NAMESPACE
