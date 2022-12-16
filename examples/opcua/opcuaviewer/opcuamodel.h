// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef OPCUAMODEL_H
#define OPCUAMODEL_H

#include "treeitem.h"

#include <QAbstractItemModel>
#include <QOpcUaClient>
#include <QOpcUaNode>

#include <memory>

class TreeItem;

class OpcUaModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    OpcUaModel(QObject *parent = nullptr);

    void setOpcUaClient(QOpcUaClient *);
    QOpcUaClient* opcUaClient() const;

    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    QOpcUaClient *mOpcUaClient = nullptr;
    std::unique_ptr<TreeItem> mRootItem;

    friend class TreeItem;
};

#endif // OPCUAMODEL_H
