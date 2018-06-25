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

#ifndef TREEITEM_H
#define TREEITEM_H

#include <QObject>
#include <QOpcUaNode>
#include <memory>

QT_BEGIN_NAMESPACE

class OpcUaModel;

class TreeItem : public QObject
{
    Q_OBJECT
public:
    explicit TreeItem(OpcUaModel *model);
    TreeItem(QOpcUaNode *node, OpcUaModel *model, TreeItem *parent);
    TreeItem(QOpcUaNode *node, OpcUaModel *model, const QOpcUaReferenceDescription &browsingData, TreeItem *parent);
    ~TreeItem();
    TreeItem *child(int row);
    int childIndex(const TreeItem *child) const;
    int childCount();
    int columnCount() const;
    QVariant data(int column);
    int row() const;
    TreeItem *parentItem();
    void appendChild(TreeItem *child);
    QPixmap icon(int column) const;
    bool hasChildNodeItem(const QString &nodeId) const;

private slots:
    void startBrowsing();
    void handleAttributes(QOpcUa::NodeAttributes attr);
    void browseFinished(QVector<QOpcUaReferenceDescription> children, QOpcUa::UaStatusCode statusCode);

protected:
    std::unique_ptr<QOpcUaNode> mOpcNode;
    OpcUaModel *mModel = nullptr;

private:
    QString variantToString(const QVariant &value, const QString &typeNodeId = QString()) const;
    QString localizedTextToString(const QOpcUa::QLocalizedText &text) const;
    QString rangeToString(const QOpcUa::QRange &range) const;
    QString euInformationToString(const QOpcUa::QEUInformation &info) const;
    QString axisStepsToString(const QVector<double> &vec) const;

    bool mAttributesReady = false;
    bool mBrowseStarted = false;
    QList<TreeItem *> mChildItems;
    QSet<QString> mChildNodeIds;
    TreeItem *mParentItem = nullptr;

private:
    QString mNodeBrowseName;
    QString mNodeId;
    QString mNodeDisplayName;
    QOpcUa::NodeClass mNodeClass = QOpcUa::NodeClass::Undefined;
};

QT_END_NAMESPACE

#endif // TREEITEM_H
