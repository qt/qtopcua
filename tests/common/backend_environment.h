// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QString>

void updateEnvironment()
{
#ifdef Q_CC_MSVC
    const QByteArray uaEnvVar("CI_UACPP_"
        #if _MSC_VER >= 1900
            "msvc2017"
        #else
            "msvc2015"
        #endif
            "_"
        #if defined(Q_OS_WIN64)
            "x64"
        #else
            "x86"
        #endif
            "_PREFIX");

    const QString uaPath = qEnvironmentVariable(uaEnvVar.constData());
    if (uaPath.isEmpty() || !QFileInfo(uaPath).exists())
        qWarning() << uaPath << " pointing to a non existing location";

    if (!qputenv("PATH", (uaPath + QLatin1String("\\bin;") + qEnvironmentVariable("PATH")).toLocal8Bit()))
        qWarning() << "Could not update PATH";
#endif
}
