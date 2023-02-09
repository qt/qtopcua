# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

#####################################################################
## tst_x509 Test:
#####################################################################

qt_internal_add_test(tst_x509
    SOURCES
        tst_x509.cpp
    LIBRARIES
        Qt::Network
        Qt::OpcUa
)
