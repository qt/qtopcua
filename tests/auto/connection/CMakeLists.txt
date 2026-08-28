# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_connection LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

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
