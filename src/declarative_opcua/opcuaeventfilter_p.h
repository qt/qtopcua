// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef OPCUAEVENTFILTER_P_H
#define OPCUAEVENTFILTER_P_H

#include <QQmlListProperty>

#include <private/opcuasimpleattributeoperand_p.h>
#include <private/opcuafilterelement_p.h>

#include <QOpcUaMonitoringParameters>
#include <QList>
#include <QtQml/qqml.h>

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

QT_BEGIN_NAMESPACE

class OpcUaNodeId;
class OpcUaRelativeNodePath;

class OpcUaEventFilter : public QObject {
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<OpcUaFilterElement> where READ filterElements)
    Q_PROPERTY(QQmlListProperty<OpcUaSimpleAttributeOperand> select READ selectors)

    QML_NAMED_ELEMENT(EventFilter)
    QML_ADDED_IN_VERSION(5, 13)

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

#endif // OPCUAEVENTFILTER_P_H
