/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

#ifndef OPCUAEVENTFILTER
#define OPCUAEVENTFILTER

#include <QQmlListProperty>

#include "opcuasimpleattributeoperand.h"
#include "opcuafilterelement.h"
#include <QOpcUaMonitoringParameters>
#include <QList>

QT_BEGIN_NAMESPACE

class OpcUaNodeId;
class OpcUaRelativeNodePath;

class OpcUaEventFilter : public QObject {
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<OpcUaFilterElement> where READ filterElements)
    Q_PROPERTY(QQmlListProperty<OpcUaSimpleAttributeOperand> select READ selectors)

public:
    explicit OpcUaEventFilter(QObject *parent = nullptr);
    ~OpcUaEventFilter();
    bool operator==(const OpcUaEventFilter &other) const;
    QOpcUaMonitoringParameters::EventFilter filter(QOpcUaClient *client) const;

    QQmlListProperty<OpcUaSimpleAttributeOperand> selectors();
    void appendSelector(OpcUaSimpleAttributeOperand *selector);
    int selectorCount() const;
    OpcUaSimpleAttributeOperand *selector(int index) const;
    void clearSelectors();

    QQmlListProperty<OpcUaFilterElement> filterElements();
    void appendFilterElement(OpcUaFilterElement *filterElement);
    int filterElementCount() const;
    OpcUaFilterElement *filterElement(int index) const;
    void clearFilterElements();

signals:
    void dataChanged();

private:
    static void appendSelector(QQmlListProperty<OpcUaSimpleAttributeOperand> *list, OpcUaSimpleAttributeOperand *);
    static qsizetype selectorCount(QQmlListProperty<OpcUaSimpleAttributeOperand> *list);
    static OpcUaSimpleAttributeOperand* selector(QQmlListProperty<OpcUaSimpleAttributeOperand> *list, qsizetype index);
    static void clearSelectors(QQmlListProperty<OpcUaSimpleAttributeOperand> *list);

    static void appendFilterElement(QQmlListProperty<OpcUaFilterElement> *list, OpcUaFilterElement *nodeId);
    static qsizetype filterElementCount(QQmlListProperty<OpcUaFilterElement> *list);
    static OpcUaFilterElement* filterElement(QQmlListProperty<OpcUaFilterElement> *list, qsizetype index);
    static void clearFilterElements(QQmlListProperty<OpcUaFilterElement> *list);

    QList<OpcUaFilterElement*> m_filterElements;
    QList<OpcUaSimpleAttributeOperand*> m_selectors;
};

QT_END_NAMESPACE

#endif // OPCUAEVENTFILTER
