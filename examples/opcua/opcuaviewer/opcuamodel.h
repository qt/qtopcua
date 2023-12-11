// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef OPCUAMODEL_H
#define OPCUAMODEL_H

#include <QAbstractItemModel>

#include <memory>

class TreeItem;

QT_BEGIN_NAMESPACE
class QOpcUaClient;
class QOpcUaGenericStructHandler;
QT_END_NAMESPACE

class OpcUaModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit OpcUaModel(QObject *parent = nullptr);
    ~OpcUaModel();

    void setOpcUaClient(QOpcUaClient *);
    void setGenericStructHandler(QOpcUaGenericStructHandler *handler);
    QOpcUaClient* opcUaClient() const;
    QOpcUaGenericStructHandler *genericStructHandler() const;

    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
    QOpcUaClient *mOpcUaClient = nullptr;
    QOpcUaGenericStructHandler *mGenericStructHandler = nullptr;
    std::unique_ptr<TreeItem> mRootItem;

    friend class TreeItem;
};

#endif // OPCUAMODEL_H
