/******************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
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
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
******************************************************************************/

#ifndef QUACPPUTILS_H
#define QUACPPUTILS_H

#include <QtCore/qstring.h>

#include <uanodeid.h>

QT_BEGIN_NAMESPACE

namespace UACppUtils {
    UaNodeId nodeIdFromQString(const QString &name);
    QString nodeIdToQString(const UaNodeId &id);
}

QT_END_NAMESPACE

#endif // QUACPPUTILS_H
