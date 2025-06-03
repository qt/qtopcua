// Copyright (C) 2025 basysKom GmbH, opensource@basyskom.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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

#ifndef QOPCUASECURITYPOLICYURIS_P_H
#define QOPCUASECURITYPOLICYURIS_P_H

#include <QtCore/qlatin1stringview.h>

QT_BEGIN_NAMESPACE

namespace QOpcUa {
static constexpr QLatin1StringView NonePolicy("http://opcfoundation.org/UA/SecurityPolicy#None");
static constexpr QLatin1StringView Basic128Rsa15Policy("http://opcfoundation.org/UA/SecurityPolicy#Basic128Rsa15");
static constexpr QLatin1StringView Basic256Policy("http://opcfoundation.org/UA/SecurityPolicy#Basic256");
static constexpr QLatin1StringView Aes256Sha256RsaPssPolicy("http://opcfoundation.org/UA/SecurityPolicy#Aes256_Sha256_RsaPss");
static constexpr QLatin1StringView Basic256Sha256Policy("http://opcfoundation.org/UA/SecurityPolicy#Basic256Sha256");
static constexpr QLatin1StringView Aes128Sha256RsaOaepPolicy("http://opcfoundation.org/UA/SecurityPolicy#Aes128_Sha256_RsaOaep");
}

QT_END_NAMESPACE

#endif // QOPCUASECURITYPOLICYURIS_P_H
