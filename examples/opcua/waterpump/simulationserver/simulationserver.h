/****************************************************************************
**
** Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the QtOpcUa module.
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

#ifndef QOPCUADEMOSERVER_H
#define QOPCUADEMOSERVER_H

#include <open62541.h>

#include <qopcuatype.h>

#include <QtCore/QDateTime>
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QVariant>

QT_BEGIN_NAMESPACE

class DemoServer : public QObject
{
    Q_OBJECT
public:
    DemoServer(QObject *parent = nullptr);
    ~DemoServer();
    bool init();

    UA_NodeId addFolder(const QString &parent, const QString &nodeString, const QString &displayName, const QString &description = QString());

    UA_NodeId addVariable(const UA_NodeId &folder, const QString &variableNode, const QString &name, const QVariant &value, QOpcUa::Types type);

    UA_NodeId addMethod(const UA_NodeId &folder, const QString &variableNode, const QString &description, const QString &name, UA_MethodCallback cb);

    static UA_StatusCode startPumpMethod(UA_Server *server, const UA_NodeId *sessionId, void *sessionHandle,
                                            const UA_NodeId *methodId, void *methodContext,
                                            const UA_NodeId *objectId, void *objectContext,
                                            size_t inputSize, const UA_Variant *input,
                                            size_t outputSize, UA_Variant *output);
    static UA_StatusCode stopPumpMethod(UA_Server *server, const UA_NodeId *sessionId, void *sessionHandle,
                                            const UA_NodeId *methodId, void *methodContext,
                                            const UA_NodeId *objectId, void *objectContext,
                                            size_t inputSize, const UA_Variant *input,
                                            size_t outputSize, UA_Variant *output);

    static UA_StatusCode flushTank2Method(UA_Server *server, const UA_NodeId *sessionId, void *sessionHandle,
                                            const UA_NodeId *methodId, void *methodContext,
                                            const UA_NodeId *objectId, void *objectContext,
                                            size_t inputSize, const UA_Variant *input,
                                            size_t outputSize, UA_Variant *output);

    static UA_StatusCode resetMethod(UA_Server *server, const UA_NodeId *sessionId, void *sessionHandle,
                                            const UA_NodeId *methodId, void *methodContext,
                                            const UA_NodeId *objectId, void *objectContext,
                                            size_t inputSize, const UA_Variant *input,
                                            size_t outputSize, UA_Variant *output);

    enum class MachineState : quint32 {
        Idle,
        Pumping,
        Flushing,
    };

    void setState(MachineState state);
    void setPercentFillTank1(double fill);
    void setPercentFillTank2(double fill);
    void setTank2ValveState(bool state);

    double readTank2TargetValue();

private:
    UA_ServerConfig *m_config{nullptr};
    UA_Server *m_server{nullptr};
    QAtomicInt m_running{false};
    QTimer m_timer;
    QTimer m_machineTimer;
    MachineState m_state;
    double m_percentFilledTank1;
    double m_percentFilledTank2;
    double m_tank2TargetPercent;
    UA_NodeId m_percentFilledTank1Node;
    UA_NodeId m_percentFilledTank2Node;
    UA_NodeId m_tank2TargetPercentNode;
    UA_NodeId m_tank2ValveStateNode;
    UA_NodeId m_machineStateNode;

public slots:
    void launch();
    void processServerEvents();
    void shutdown();
};

QT_END_NAMESPACE

#endif // QOPCUADEMOSERVER_H
