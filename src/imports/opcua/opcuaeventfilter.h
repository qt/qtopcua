/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
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

#ifndef OPCUAEVENTFILTER
#define OPCUAEVENTFILTER

#include <QQmlListProperty>

#include "opcuasimpleattributeoperand.h"
#include "opcuafilterelement.h"
#include <QOpcUaMonitoringParameters>

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
    static int selectorCount(QQmlListProperty<OpcUaSimpleAttributeOperand> *list);
    static OpcUaSimpleAttributeOperand* selector(QQmlListProperty<OpcUaSimpleAttributeOperand> *list, int index);
    static void clearSelectors(QQmlListProperty<OpcUaSimpleAttributeOperand> *list);

    static void appendFilterElement(QQmlListProperty<OpcUaFilterElement> *list, OpcUaFilterElement *nodeId);
    static int filterElementCount(QQmlListProperty<OpcUaFilterElement> *list);
    static OpcUaFilterElement* filterElement(QQmlListProperty<OpcUaFilterElement> *list, int index);
    static void clearFilterElements(QQmlListProperty<OpcUaFilterElement> *list);

    QVector<OpcUaFilterElement*> m_filterElements;
    QVector<OpcUaSimpleAttributeOperand*> m_selectors;
};

QT_END_NAMESPACE

#endif // OPCUAEVENTFILTER
