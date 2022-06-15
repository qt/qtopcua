// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QOPCUALOCALIZEDTEXT_H
#define QOPCUALOCALIZEDTEXT_H

#include <QtOpcUa/qopcuaglobal.h>
#include <QtCore/qobject.h>
#include <QtCore/qshareddata.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

class QOpcUaLocalizedTextData;
class Q_OPCUA_EXPORT QOpcUaLocalizedText
{
    Q_GADGET
    Q_PROPERTY(QString locale READ locale WRITE setLocale)
    Q_PROPERTY(QString text READ text WRITE setText)

public:
    QOpcUaLocalizedText();
    QOpcUaLocalizedText(const QOpcUaLocalizedText &);
    QOpcUaLocalizedText(const QString &locale, const QString &text);
    QOpcUaLocalizedText &operator=(const QOpcUaLocalizedText &);
    bool operator==(const QOpcUaLocalizedText &rhs) const;
    operator QVariant() const;
    ~QOpcUaLocalizedText();

    QString locale() const;
    void setLocale(const QString &locale);

    QString text() const;
    void setText(const QString &text);

private:
    QSharedDataPointer<QOpcUaLocalizedTextData> data;

#ifndef QT_NO_DEBUG_STREAM
    friend Q_OPCUA_EXPORT QDebug operator<<(QDebug debug, const QOpcUaLocalizedText &lt);
#endif
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QOpcUaLocalizedText)

#endif // QOPCUALOCALIZEDTEXT_H
