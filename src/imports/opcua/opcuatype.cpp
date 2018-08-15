/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
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

#include "opcuatype.h"

QT_BEGIN_NAMESPACE

/*!
    \qmltype LocalizedText
    \inqmlmodule QtOpcUa
    \brief Contains a text with associated locale.
    \since QtOpcUa 5.12

    The two members of this type contain the actual text and the locale of the text.
*/

/*!
    \qmlproperty string LocalizedText::text

    Textual content.
*/

/*!
    \qmlproperty string LocalizedText::locale

    Locale of the contained text.
    This has to be in a modified ISO standard notation, for example \c en-US.
    See OPC UA specification part 3, 8.4 for details.
*/

LocalizedText::LocalizedText()
{
}

LocalizedText::LocalizedText(const QOpcUa::QLocalizedText &other)
    : QLocalizedText(other)
{
}

LocalizedText &LocalizedText::operator=(const LocalizedText &rhs)
{
    if (this != &rhs)
        QLocalizedText::operator =(rhs);
    return *this;
}

QT_END_NAMESPACE

