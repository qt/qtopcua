// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef OPCUAFOREIGN_P_H
#define OPCUAFOREIGN_P_H

#include <QLoggingCategory>
#include <qqml.h>
#include <QtOpcUa/qopcuatype.h>
#include <QtOpcUa/qopcuausertokenpolicy.h>
#include <QtOpcUa/qopcuaapplicationdescription.h>
#include <QtOpcUa/qopcuausertokenpolicy.h>
#include <QtCore/private/qglobal_p.h>

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

namespace Constants
{
    Q_NAMESPACE
    QML_FOREIGN_NAMESPACE(QOpcUa)
    QML_NAMESPACE_EXTENDED(QOpcUaUserTokenPolicy)
    QML_ELEMENT
    QML_ADDED_IN_VERSION(5, 12)
};

namespace QOpcUaApplicationDescriptionForeign
{
    Q_NAMESPACE
    QML_FOREIGN_NAMESPACE(QOpcUaApplicationDescription)
    QML_NAMED_ELEMENT(ApplicationDescription)
    QML_ADDED_IN_VERSION(5, 13)
};

namespace QOpcUaUserTokenPolicyForeign
{
    Q_NAMESPACE
    QML_FOREIGN_NAMESPACE(QOpcUaUserTokenPolicy)
    QML_NAMED_ELEMENT(UserTokenPolicy)
    QML_ADDED_IN_VERSION(5, 13)
};

QT_END_NAMESPACE

#endif // OPCUAFOREIGN_P_H
