/****************************************************************************
**
** Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtOpcUa module of the Qt Toolkit.
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

#ifndef QOPCUAEVENTFILTERRESULT_H
#define QOPCUAEVENTFILTERRESULT_H

#include <QtOpcUa/qopcuatype.h>

#include <QtCore/qshareddata.h>

QT_BEGIN_NAMESPACE

class QOpcUaContentFilterElementResult;

class QOpcUaEventFilterResultData;
class Q_OPCUA_EXPORT QOpcUaEventFilterResult
{
public:
    QOpcUaEventFilterResult();
    QOpcUaEventFilterResult(const QOpcUaEventFilterResult &);
    QOpcUaEventFilterResult &operator=(const QOpcUaEventFilterResult &);
    ~QOpcUaEventFilterResult();

    bool isGood() const;

    QList<QOpcUa::UaStatusCode> selectClauseResults() const;
    QList<QOpcUa::UaStatusCode> &selectClauseResultsRef();
    void setSelectClauseResults(const QList<QOpcUa::UaStatusCode> &selectClausesResult);

    QList<QOpcUaContentFilterElementResult> whereClauseResults() const;
    QList<QOpcUaContentFilterElementResult> &whereClauseResultsRef();
    void setWhereClauseResults(const QList<QOpcUaContentFilterElementResult> &whereClauseResult);

private:
    QSharedDataPointer<QOpcUaEventFilterResultData> data;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaEventFilterResult)

#endif // QOPCUAEVENTFILTERRESULT_H
