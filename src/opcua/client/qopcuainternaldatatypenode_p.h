// Copyright (C) 2023 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

#ifndef QOPCUAINTERNALDATATYPENODE_H
#define QOPCUAINTERNALDATATYPENODE_H

#include <QtOpcUa/qopcuaclient.h>

#include <QtCore/qpointer.h>

QT_BEGIN_NAMESPACE

class QOpcUaInternalDataTypeNode : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY(QOpcUaInternalDataTypeNode)
public:
    QOpcUaInternalDataTypeNode(QOpcUaClient *client);

    bool initialize(const QString &nodeId);

    QVariant definition() const;

    bool isAbstract() const;

    QString name() const;

    QString nodeId() const;

    const std::vector<std::unique_ptr<QOpcUaInternalDataTypeNode>> &children() const;

Q_SIGNALS:
    void initializeFinished(bool success);

protected:
    void handleFinished(bool success);

private:

    QPointer<QOpcUaClient> m_client;
    QScopedPointer<QOpcUaNode> m_node;
    std::vector<std::unique_ptr<QOpcUaInternalDataTypeNode>> m_children;

    QVariant m_definition;
    bool m_isAbstract = false;
    QString m_name;
    QString m_nodeId;

    size_t m_finishedCount = 0;
    bool m_hasError = false;
};

QT_END_NAMESPACE

#endif // QOPCUAINTERNALDATATYPENODE_H
