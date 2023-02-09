# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause


#####################################################################
## tst_gds Test:
#####################################################################

qt_internal_add_test(tst_gds
    SOURCES
        tst_gds.cpp
    LIBRARIES
        Qt::Network
        Qt::OpcUa
)
