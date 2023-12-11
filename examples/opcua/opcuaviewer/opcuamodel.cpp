// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "opcuamodel.h"
#include "treeitem.h"

#include <QOpcUaClient>
#include <QIcon>

OpcUaModel::OpcUaModel(QObject *parent) : QAbstractItemModel(parent)
{
}

OpcUaModel::~OpcUaModel()
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

void OpcUaModel::setGenericStructHandler(QOpcUaGenericStructHandler *handler)
{
    mGenericStructHandler = handler;
}

QOpcUaClient *OpcUaModel::opcUaClient() const
{
    return mOpcUaClient;
}

QOpcUaGenericStructHandler *OpcUaModel::genericStructHandler() const
{
    return mGenericStructHandler;
}

QVariant OpcUaModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto item = static_cast<TreeItem *>(index.internalPointer());

    switch (role) {
    case Qt::DisplayRole:
        return item->data(index.column());
    case Qt::DecorationRole:
        if (index.column() == 0)
            return item->icon(index.column());
        break;
    default:
        break;
    }

    return QVariant();
}

QVariant OpcUaModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    using namespace Qt::Literals::StringLiterals;

    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Vertical)
        return u"Row %1"_s.arg(section);

    switch (section) {
    case 0:
        return u"BrowseName"_s;
    case 1:
        return u"Value"_s;
    case 2:
        return u"NodeClass"_s;
    case 3:
        return u"DataType"_s;
    case 4:
        return u"NodeId"_s;
    case 5:
        return u"DisplayName"_s;
    case 6:
        return u"Description"_s;
    case 7:
        return u"Historizing"_s;
    default:
        break;
    }
    return u"Column %1"_s.arg(section);
}

QModelIndex OpcUaModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *item = parent.isValid()
        ? static_cast<TreeItem*>(parent.internalPointer())->child(row)
        : mRootItem.get();

    return item ? createIndex(row, column, item) : QModelIndex();
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

    if (!mOpcUaClient)
        return 0;

    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        return 1; // only one root item

    auto parentItem = static_cast<TreeItem*>(parent.internalPointer());
    return parentItem ? parentItem->childCount() : 0;
}

int OpcUaModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    return mRootItem ? mRootItem->columnCount() : 0;
}
