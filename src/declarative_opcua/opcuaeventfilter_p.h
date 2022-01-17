/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt OPC UA module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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