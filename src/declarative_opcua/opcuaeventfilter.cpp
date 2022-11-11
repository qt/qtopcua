// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <private/opcuaeventfilter_p.h>

QT_BEGIN_NAMESPACE

/*!
    \qmltype EventFilter
    \inqmlmodule QtOpcUa
    \brief Defines an EventFilter for a monitored item.
    \since QtOpcUa 5.13

    An event filter is required for monitoring events on the server.
    It consists of \c select clauses and a \c where clause.

    The \c select clauses are used to specify the data the user wants to receive when an event occurs.
    It consists of \l {SimpleAttributeOperand} simple attribute operands which select
    attributes of child nodes of an event type, for example the value attribute of the "Message"
    property of BaseEventType.

    The \c where clause is used to restrict the reported events by matching against certain criteria.
    Several operators and four different operand types allow filtering based on the values of the
    attributes of the child nodes of an event type.

    The select clause consists of an array of \l SimpleAttributeOperand.
    The where clause consists of an array of \l SimpleAttributeOperand, \l LiteralOperand, \l ElementOperand or \l AttributeOperand.

    The following EventFilter tells the server to report the value of the "Message" field for events that have a "Severity" field with value >= 500:

    \code
        import QtOpcUa as QtOpcUa
        QtOpcUa.Node {
            ...

            eventFilter: QtOpcUa.EventFilter {
                select: [
                          QtOpcUa.SimpleAttributeOperand {
                            browsePath: [
                                          QtOpcUa.NodeId {
                                              identifier: "Severity"
                                              ns: "http://opcfoundation.org/UA/"
                                          }
                                        ]
                          },
                          QtOpcUa.SimpleAttributeOperand {
                            browsePath: [
                                          QtOpcUa.NodeId {
                                              identifier: "Message"
                                              ns: "http://opcfoundation.org/UA/"
                                          }
                                        ]
                          }
                ]

                where: [
                    QtOpcUa.FilterElement {
                        operator: QtOpcUa.FilterElement.GreaterThanOrEqual
                        firstOperand: QtOpcUa.SimpleAttributeOperand {
                            browsePath: [
                                          QtOpcUa.NodeId {
                                              identifier: "Severity"
                                              ns: "http://opcfoundation.org/UA/"
                                          }
                                        ]
                        }
                        secondOperand: QtOpcUa.LiteralOperand {
                                        value: 700
                                        type: QtOpcUa.Constants.UInt16
                         }
                    }
                ]
            }
        }
    \endcode

    For a more complex example with two conditions, see \l QOpcUaElementOperand.

    \sa FilterElement
*/

/*!
    \qmlproperty list<FilterElement> EventFilter::where

    Content filter used to restrict the reported events to events matching certain criteria.
*/

/*!
    \qmlproperty list<SimpleAttributeOperand> EventFilter::select

    Selected event fields that shall be included when a new event is reported.
*/

OpcUaEventFilter::OpcUaEventFilter(QObject *parent)
    : QObject(parent)
{
}

OpcUaEventFilter::~OpcUaEventFilter() = default;

bool OpcUaEventFilter::operator==(const OpcUaEventFilter &other) const
{
    return this->m_filterElements == other.m_filterElements
            && this->m_selectors == other.m_selectors;
}

QOpcUaMonitoringParameters::EventFilter OpcUaEventFilter::filter(QOpcUaClient *client) const
{
    QOpcUaMonitoringParameters::EventFilter filterValue;
    for (const auto &i : std::as_const(m_selectors))
        filterValue.selectClausesRef().append(i->toSimpleAttributeOperand(client));

    for (const auto &i : std::as_const(m_filterElements))
        filterValue.whereClauseRef().append(i->toFilterElement(client));

    return filterValue;
}

QQmlListProperty<OpcUaFilterElement> OpcUaEventFilter::filterElements()
{
    return QQmlListProperty<OpcUaFilterElement>(this, this,
                                         &OpcUaEventFilter::appendFilterElement,
                                         &OpcUaEventFilter::filterElementCount,
                                         &OpcUaEventFilter::filterElement,
                                         &OpcUaEventFilter::clearFilterElements);
}

void OpcUaEventFilter::appendFilterElement(OpcUaFilterElement *nodeId)
{
    m_filterElements.append(nodeId);
    emit dataChanged();
}

int OpcUaEventFilter::filterElementCount() const
{
    return m_filterElements.size();
}

OpcUaFilterElement *OpcUaEventFilter::filterElement(int index) const
{
    return m_filterElements.at(index);
}

void OpcUaEventFilter::clearFilterElements()
{
    m_filterElements.clear();
    emit dataChanged();
}

void OpcUaEventFilter::appendFilterElement(QQmlListProperty<OpcUaFilterElement> *list, OpcUaFilterElement *nodeId)
{
    reinterpret_cast<OpcUaEventFilter*>(list->data)->appendFilterElement(nodeId);
}

qsizetype OpcUaEventFilter::filterElementCount(QQmlListProperty<OpcUaFilterElement> *list)
{
    return reinterpret_cast<OpcUaEventFilter*>(list->data)->filterElementCount();
}

OpcUaFilterElement *OpcUaEventFilter::filterElement(QQmlListProperty<OpcUaFilterElement> *list, qsizetype index)
{
    return reinterpret_cast<OpcUaEventFilter*>(list->data)->filterElement(index);
}

void OpcUaEventFilter::clearFilterElements(QQmlListProperty<OpcUaFilterElement> *list)
{
    reinterpret_cast<OpcUaEventFilter*>(list->data)->clearFilterElements();
}

QQmlListProperty<OpcUaSimpleAttributeOperand> OpcUaEventFilter::selectors()
{
    return QQmlListProperty<OpcUaSimpleAttributeOperand>(this, this,
                                         &OpcUaEventFilter::appendSelector,
                                         &OpcUaEventFilter::selectorCount,
                                         &OpcUaEventFilter::selector,
                                         &OpcUaEventFilter::clearSelectors);
}

void OpcUaEventFilter::appendSelector(OpcUaSimpleAttributeOperand *nodeId)
{
    m_selectors.append(nodeId);
    emit dataChanged();
}

int OpcUaEventFilter::selectorCount() const
{
    return m_selectors.size();
}

OpcUaSimpleAttributeOperand *OpcUaEventFilter::selector(int index) const
{
    return m_selectors.at(index);
}

void OpcUaEventFilter::clearSelectors()
{
    m_selectors.clear();
    emit dataChanged();
}

void OpcUaEventFilter::appendSelector(QQmlListProperty<OpcUaSimpleAttributeOperand> *list, OpcUaSimpleAttributeOperand *nodeId)
{
    reinterpret_cast<OpcUaEventFilter*>(list->data)->appendSelector(nodeId);
}

qsizetype OpcUaEventFilter::selectorCount(QQmlListProperty<OpcUaSimpleAttributeOperand> *list)
{
    return reinterpret_cast<OpcUaEventFilter*>(list->data)->selectorCount();
}

OpcUaSimpleAttributeOperand *OpcUaEventFilter::selector(QQmlListProperty<OpcUaSimpleAttributeOperand> *list, qsizetype index)
{
    return reinterpret_cast<OpcUaEventFilter*>(list->data)->selector(index);
}

void OpcUaEventFilter::clearSelectors(QQmlListProperty<OpcUaSimpleAttributeOperand> *list)
{
    reinterpret_cast<OpcUaEventFilter*>(list->data)->clearSelectors();
}

QT_END_NAMESPACE
