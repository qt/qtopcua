# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

#####################################################################
## tst_connection Test:
#####################################################################

qt_internal_add_test(tst_connection
    SOURCES
        tst_connection.cpp
    LIBRARIES
        Qt::OpcUa
)

if (WIN32)
    target_compile_definitions(tst_connection PRIVATE TESTS_CMAKE_SPECIFIC_PATH)
endif()
