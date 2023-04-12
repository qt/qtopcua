# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)

include(.cmake.conf)
project(QtOpcUa
    VERSION "${QT_REPO_MODULE_VERSION}"
    DESCRIPTION "Qt OpcUa Libraries"
    HOMEPAGE_URL "https://qt.io/"
    LANGUAGES CXX C
)

find_package(Qt6 ${PROJECT_VERSION} CONFIG REQUIRED COMPONENTS BuildInternals Core Network)
find_package(Qt6 ${PROJECT_VERSION} CONFIG OPTIONAL_COMPONENTS Gui Quick QuickTest Widgets)
qt_internal_project_setup()

# Needed for qt_opcua_disable_optimizations_in_current_dir.
include(src/opcua/Qt6OpcUaMacros.cmake)

if(QNX)
    message(NOTICE "Skipping the build as the condition \"NOT QNX\" is not met.")
    return()
endif()
if(WASM)
    message(NOTICE "Skipping the build as the condition \"NOT WASM\" is not met.")
    return()
endif()
qt_build_repo()
