// Copyright (C) 2015 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qopcualocalizedtext.h"

QT_BEGIN_NAMESPACE

/*!
    \class QOpcUaLocalizedText
    \inmodule QtOpcUa
    \brief The OPC UA LocalizedText type.

    This is the Qt OPC UA representation for the OPC UA LocalizedText type defined in OPC-UA part 3, 8.5.
    A LocalizedText value contains a text string with associated locale information in a second string (e. g. "en" or "en-US").
    The format of the locale information string is <language>[-<country/region>]. Language is usually given as ISO 639 two letter code,
    country/region as ISO 3166 two letter code. Custom codes are also allowed (see OPC-UA part 3, 8.4).
    It can be used to provide multiple text strings in different languages for a value using an array of LocalizedText elements.
*/

/*!
    \qmltype LocalizedText
    \inqmlmodule QtOpcUa
    \brief Contains a text with associated locale.
    \since QtOpcUa 5.12

    The two members of this type contain the actual text and the locale of the text.
*/

/*!
    \property QOpcUaLocalizedText::text

    Textual content.
*/

/*!
    \qmlproperty string LocalizedText::text

    Textual content.
*/

/*!
    \property QOpcUaLocalizedText::locale

    Locale of the contained text.
    This has to be in a modified ISO standard notation, for example \c en-US.
    See OPC UA specification part 3, 8.4 for details.
*/

/*!
    \qmlproperty string LocalizedText::locale

    Locale of the contained text.
    This has to be in a modified ISO standard notation, for example \c en-US.
    See OPC UA specification part 3, 8.4 for details.
*/

class QOpcUaLocalizedTextData : public QSharedData
{
public:
    QString locale;
    QString text;
};

/*!
    Default constructs a localized text with no parameters set.
*/
QOpcUaLocalizedText::QOpcUaLocalizedText()
    : data(new QOpcUaLocalizedTextData)
{
}

/*!
    Constructs a localized text from \a rhs.
*/
QOpcUaLocalizedText::QOpcUaLocalizedText(const QOpcUaLocalizedText &rhs)
    : data(rhs.data)
{
}

/*!
    Constructs a localized text with the locale \a locale and the text \a text.
*/
QOpcUaLocalizedText::QOpcUaLocalizedText(const QString &locale, const QString &text)
    : data(new QOpcUaLocalizedTextData)
{
    data->locale = locale;
    data->text = text;
}

/*!
    Sets the values from \a rhs in this localized text.
*/
QOpcUaLocalizedText &QOpcUaLocalizedText::operator=(const QOpcUaLocalizedText &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

/*!
    Returns \c true if this localized text has the same value as \a rhs.
*/
bool QOpcUaLocalizedText::operator==(const QOpcUaLocalizedText &rhs) const
{
    return data->locale == rhs.locale() &&
            data->text == rhs.text();
}

/*!
    Converts this localized text to \l QVariant.
*/
QOpcUaLocalizedText::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

QOpcUaLocalizedText::~QOpcUaLocalizedText()
{
}

/*!
    Returns the text.
*/
QString QOpcUaLocalizedText::text() const
{
    return data->text;
}

/*!
    Sets the text to \a text.
*/
void QOpcUaLocalizedText::setText(const QString &text)
{
    data->text = text;
}

/*!
    Returns the locale.
*/
QString QOpcUaLocalizedText::locale() const
{
    return data->locale;
}

/*!
    Sets the locale to \a locale.
*/
void QOpcUaLocalizedText::setLocale(const QString &locale)
{
    data->locale = locale;
}

#ifndef QT_NO_DEBUG_STREAM

/*!
    \fn QDebug QOpcUaLocalizedText::operator<<(QDebug debug, const QOpcUaLocalizedText &text)
    \since 6.3

    Writes the localized \a text to the \a debug output.

    \sa QDebug
*/
QDebug operator<<(QDebug debug, const QOpcUaLocalizedText &lt)
{
    QDebugStateSaver saver(debug);
    debug.nospace().quote() << "QOpcUaLocalizedText(" << lt.locale() << ", " << lt.text() << ")";
    return debug;
}

#endif

QT_END_NAMESPACE
