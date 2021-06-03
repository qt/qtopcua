/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
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

#ifndef OPCUAFOREIGN_P_H
#define OPCUAFOREIGN_P_H

#include <QLoggingCategory>
#include <qqml.h>
#include <QtOpcUa/qopcuatype.h>
#include <QtOpcUa/qopcuausertokenpolicy.h>
#include <QtOpcUa/qopcuaapplicationdescription.h>
#include <QtOpcUa/qopcuausertokenpolicy.h>

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
    QML_UNCREATABLE("This type can not be created.")
    QML_ADDED_IN_VERSION(5, 13)
};

namespace QOpcUaUserTokenPolicyForeign
{
    Q_NAMESPACE
    QML_FOREIGN_NAMESPACE(QOpcUaUserTokenPolicy)
    QML_NAMED_ELEMENT(UserTokenPolicy)
    QML_UNCREATABLE("This type can not be created.")
    QML_ADDED_IN_VERSION(5, 13)
};

QT_END_NAMESPACE

#endif // OPCUAFOREIGN_P_H
