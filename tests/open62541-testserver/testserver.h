/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
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

#ifndef TESTSERVER_H
#define TESTSERVER_H

#include <qopen62541.h>
#include <QtOpcUa/qopcuatype.h>

#include <QtCore/QDateTime>
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QVariant>
#include <QtCore/QVector>

QT_BEGIN_NAMESPACE

class TestServer : public QObject
{
    Q_OBJECT
public:
    explicit TestServer(QObject *parent = nullptr);
    ~TestServer();
    bool init();
    bool createInsecureServerConfig(UA_ServerConfig *config);
#if defined UA_ENABLE_ENCRYPTION
    bool createSecureServerConfig(UA_ServerConfig *config);
#endif

    int registerNamespace(const QString &ns);
    UA_NodeId addFolder(const QString &nodeString, const QString &displayName, const QString &description = QString());
    UA_NodeId addObject(const UA_NodeId &folderId, int namespaceIndex, const QString &objectName = QString());

    UA_NodeId addVariable(const UA_NodeId &folder, const QString &variableNode, const QString &name, const QVariant &value,
                          QOpcUa::Types type, QVector<quint32> arrayDimensions = QVector<quint32>(), int valueRank = UA_VALUERANK_ANY);
    UA_NodeId addVariableWithWriteMask(const UA_NodeId &folder, const QString &variableNode, const QString &name, const QVariant &value,
                          QOpcUa::Types type, quint32 writeMask);
    UA_NodeId addEmptyArrayVariable(const UA_NodeId &folder, const QString &variableNode, const QString &name);

    UA_NodeId addMultiplyMethod(const UA_NodeId &folder, const QString &variableNode, const QString &description);
    UA_NodeId addMultipleOutputArgumentsMethod(const UA_NodeId &folder, const QString &variableNode, const QString &description);
    UA_NodeId addAddNamespaceMethod(const UA_NodeId &folder, const QString &variableNode, const QString &description);
    UA_NodeId addNodeWithFixedTimestamp(const UA_NodeId &folder, const QString &nodeId, const QString &displayName);

    static UA_StatusCode multiplyMethod(UA_Server *server, const UA_NodeId *sessionId, void *sessionHandle,
                                            const UA_NodeId *methodId, void *methodContext,
                                            const UA_NodeId *objectId, void *objectContext,
                                            size_t inputSize, const UA_Variant *input,
                                            size_t outputSize, UA_Variant *output);

    static UA_StatusCode multipleOutputArgumentsMethod(UA_Server *server, const UA_NodeId *sessionId, void *sessionHandle,
                                            const UA_NodeId *methodId, void *methodContext,
                                            const UA_NodeId *objectId, void *objectContext,
                                            size_t inputSize, const UA_Variant *input,
                                            size_t outputSize, UA_Variant *output);

    static UA_StatusCode addNamespaceMethod(UA_Server *server, const UA_NodeId *sessionId, void *sessionHandle,
                                            const UA_NodeId *methodId, void *methodContext, const UA_NodeId *objectId,
                                            void *objectContext, size_t inputSize, const UA_Variant *input, size_t outputSize,
                                            UA_Variant *output);


    UA_ServerConfig *m_config{nullptr};
    UA_Server *m_server{nullptr};
    QAtomicInt m_running{false};
    QTimer m_timer;

public slots:
    void launch();
    void processServerEvents();
    void shutdown();
};

QT_END_NAMESPACE

#endif // TESTSERVER_H
