// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
