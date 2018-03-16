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

#ifndef OPCUAMACHINEBACKEND_H
#define OPCUAMACHINEBACKEND_H

#include <QtOpcUa>

#include <QObject>

class OpcUaMachineBackend : public QObject
{
    Q_OBJECT

public:

    enum class MachineState : quint32 {
        Idle,
        Pumping,
        Flushing
    };
    Q_ENUM(MachineState)

    Q_PROPERTY(double percentFilledTank1 READ percentFilledTank1 NOTIFY percentFilledTank1Changed)
    Q_PROPERTY(double percentFilledTank2 READ percentFilledTank2 NOTIFY percentFilledTank2Changed)
    Q_PROPERTY(double tank2TargetPercent READ tank2TargetPercent NOTIFY tank2TargetPercentChanged)
    Q_PROPERTY(OpcUaMachineBackend::MachineState machineState READ machineState NOTIFY machineStateChanged)
    Q_PROPERTY(bool tank2ValveState READ tank2ValveState NOTIFY tank2ValveStateChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(QString machineDesignation READ machineDesignation NOTIFY machineDesignationChanged)
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)
    Q_PROPERTY(QStringList backends READ backends NOTIFY backendsChanged)

    OpcUaMachineBackend(QObject *parent = nullptr);
    ~OpcUaMachineBackend();

    double percentFilledTank1() const;
    double percentFilledTank2() const;
    double tank2TargetPercent() const;
    bool tank2ValveState() const;
    MachineState machineState() const;
    bool connected() const;
    QString machineDesignation() const;
    QString message() const;
    bool successfullyCreated() const;
    void setMessage(const QString &message);
    QStringList backends() const;

    Q_INVOKABLE void connectToEndpoint(const QString &url, qint32 index);
    Q_INVOKABLE void disconnectFromEndpoint();
    Q_INVOKABLE void machineWriteTank2TargetPercent(double value);
    Q_INVOKABLE void startPump();
    Q_INVOKABLE void stopPump();
    Q_INVOKABLE void flushTank2();
    Q_INVOKABLE void resetSimulation();

public slots:
    void clientStateHandler(QOpcUaClient::ClientState state);
    void machineStateUpdated(QOpcUa::NodeAttribute attr, const QVariant &value);
    void percentFilledTank1Updated(QOpcUa::NodeAttribute attr, const QVariant &value);
    void percentFilledTank2Updated(QOpcUa::NodeAttribute attr, const QVariant &value);
    void tank2TargetPercentUpdated(QOpcUa::NodeAttribute attr, const QVariant &value);
    void tank2ValveStateUpdated(QOpcUa::NodeAttribute attr, const QVariant &value);
    void machineDesignationRead(QOpcUa::NodeAttributes attr);
    void setpointWritten(QOpcUa::NodeAttribute attr, QOpcUa::UaStatusCode status);
    void handleMethodResult(QString methodNodeId, const QVariant &result, QOpcUa::UaStatusCode statusCode);
    void enableMonitoringFinished(QOpcUa::NodeAttribute attr, QOpcUa::UaStatusCode status);

signals:
    void percentFilledTank1Changed(double value);
    void percentFilledTank2Changed(double value);
    void tank2TargetPercentChanged(double value);
    void tank2ValveStateChanged(bool state);
    void machineStateChanged(OpcUaMachineBackend::MachineState state);
    void connectedChanged(bool connected);
    void machineDesignationChanged(QString designation);
    void messageChanged(QString message);
    void backendsChanged(QStringList backends);

private:
    void setBackends(const QStringList &backends);

    QScopedPointer<QOpcUaClient> m_client;
    QScopedPointer<QOpcUaNode> m_machineStateNode;
    QScopedPointer<QOpcUaNode> m_percentFilledTank1Node;
    QScopedPointer<QOpcUaNode> m_percentFilledTank2Node;
    QScopedPointer<QOpcUaNode> m_tank2TargetPercentNode;
    QScopedPointer<QOpcUaNode> m_tank2ValveStateNode;
    QScopedPointer<QOpcUaNode> m_machineNode;
    QScopedPointer<QOpcUaNode> m_machineDesignationNode;
    double m_percentFilledTank1;
    double m_percentFilledTank2;
    double m_tank2TargetPercent;
    bool m_tank2ValveState;
    MachineState m_machineState;
    QString m_machineDesignation;

    bool m_connected;
    QString m_message;

    bool m_successfullyCreated;
    QStringList m_backends;
};

Q_DECLARE_METATYPE(OpcUaMachineBackend::MachineState)

#endif // OPCUAMACHINEBACKEND_H
