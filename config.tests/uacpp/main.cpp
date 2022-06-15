// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <stdio.h>

#include <uaplatformlayer.h>
#include <uastring.h>
#include <uasession.h>

using namespace UaClientSdk;

int main(int /*argc*/, char ** /*argv*/)
{

    UaPlatformLayer::init();
    UaSession *session = new UaSession;

    UaPlatformLayer::cleanup();
    return 0;
}
