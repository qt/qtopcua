// Copyright (C) 2018 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef QOPCUADEMOSERVER_H
#define QOPCUADEMOSERVER_H

#include <qopen62541.h>

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

    UA_NodeId addObject(const QString &parent, const QString &nodeString, const QString &browseName,
                        const QString &displayName, const QString &description = QString(),
                        quint32 referenceType = UA_NS0ID_HASCOMPONENT);

    UA_NodeId addVariable(const UA_NodeId &folder, const QString &variableNode, const QString &browseName,
                          const QString &displayName, const QVariant &value, QOpcUa::Types type,
                          quint32 referenceType = UA_NS0ID_HASCOMPONENT);

    UA_NodeId addMethod(const UA_NodeId &folder, const QString &variableNode, const QString &description,
                        const QString &browseName, const QString &displayName, UA_MethodCallback cb,
                        quint32 referenceType = UA_NS0ID_HASCOMPONENT);

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
