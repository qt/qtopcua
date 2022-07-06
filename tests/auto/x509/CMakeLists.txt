# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

# Generated from x509.pro.

#####################################################################
## tst_x509 Test:
#####################################################################

qt_internal_add_test(tst_x509
    SOURCES
        tst_x509.cpp
    PUBLIC_LIBRARIES
        Qt::Network
        Qt::OpcUa
)
