# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_opcua LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

#####################################################################
## tst_opcua Test:
#####################################################################

qt_internal_add_test(tst_opcua
    GUI
    QMLTEST
    QML_IMPORTPATH
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/plugins/declarative"
    SOURCES
        tst_opcua.cpp
    LIBRARIES
        Qt::Gui
        Qt::OpcUaPrivate
        Qt::Qml
)

## Scopes:
#####################################################################

qt_internal_extend_target(tst_opcua CONDITION QT_FEATURE_open62541_security
    DEFINES
        SERVER_SUPPORTS_SECURITY
)

if (WIN32)
    target_compile_definitions(tst_opcua PRIVATE TESTS_CMAKE_SPECIFIC_PATH)
endif()
