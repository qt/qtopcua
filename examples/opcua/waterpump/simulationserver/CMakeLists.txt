# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(simulationserver LANGUAGES CXX C)

if (ANDROID)
    message(FATAL_ERROR "This project cannot be built on Android.")
endif()

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/opcua/waterpump/simulationserver")

find_package(Qt6 REQUIRED COMPONENTS Core OpcUa)

qt_standard_project_setup()

if (EXISTS ../../../../../${Qt6_VERSION}/Src/qtopcua/)
    set(MODULE_SOURCE ../../../../../${Qt6_VERSION}/Src/qtopcua/)
else()
    # Build from source tree
    set(MODULE_SOURCE ../../../../)
endif()

qt_add_executable(simulationserver
    ${MODULE_SOURCE}/src/plugins/opcua/open62541/qopen62541utils.cpp
    ${MODULE_SOURCE}/src/plugins/opcua/open62541/qopen62541valueconverter.cpp
    main.cpp
    simulationserver.cpp simulationserver.h
)

set_target_properties(simulationserver PROPERTIES
    WIN32_EXECUTABLE FALSE
    MACOSX_BUNDLE TRUE
)

target_include_directories(simulationserver PRIVATE
    ${MODULE_SOURCE}/src/plugins/opcua/open62541
)

target_link_libraries(simulationserver PRIVATE
    Qt6::Core
    Qt6::OpcUa
    Qt6::OpcUaPrivate
)

if (QT_FEATURE_open62541 AND NOT QT_FEATURE_system_open62541)
    target_sources(simulationserver PRIVATE
        ${MODULE_SOURCE}/src/3rdparty/open62541/open62541.h
        ${MODULE_SOURCE}/src/3rdparty/open62541/open62541.c
    )
    target_include_directories(simulationserver PRIVATE
        ${MODULE_SOURCE}/src/3rdparty/open62541
    )
endif()

# We need to disable optimizations on MSVC, to circumvent buggy code in the open62541.c file.
# Otherwise the test server fails to launch on Windows MSVC with UA_STATUSCODE_BADOUTOFMEMORY.
# We also disable them on Unixy platforms, to mimic the qmake fix done in QTBUG-75020.
# See QTBUG-85939 for details.
qt_opcua_disable_optimizations_in_current_dir()

if (QT_FEATURE_open62541 AND NOT QT_FEATURE_system_open62541)
    if (NOT (WINRT OR WIN32 AND MSVC))
        set_source_files_properties(${MODULE_SOURCE}/src/3rdparty/open62541/open62541.c
        PROPERTIES COMPILE_OPTIONS
            "-Wno-unused-parameter;-Wno-unused-function;-Wno-format;-Wno-format-security;-Wno-strict-aliasing;-Wno-unused-result;-std=c99")
    elseif(WIN32 AND MSVC)
        # CMake uses C11+ by default and the _Static_assert expansion fails during compilation
        set_source_files_properties(${MODULE_SOURCE}/src/3rdparty/open62541/open62541.c
            PROPERTIES LANGUAGE CXX)
        set_source_files_properties(${MODULE_SOURCE}/src/3rdparty/open62541/open62541.c
            PROPERTIES COMPILE_OPTIONS
                "/Zc:strictStrings-;/permissive;/wd4200")
    endif()
endif()

if(QT_FEATURE_system_open62541 OR NOT QT_FEATURE_open62541)
    target_link_libraries(simulationserver PRIVATE
        open62541
    )
endif()

install(TARGETS simulationserver
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
