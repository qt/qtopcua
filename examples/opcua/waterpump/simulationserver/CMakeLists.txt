# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(simulationserver LANGUAGES CXX C)

if (ANDROID)
    message(FATAL_ERROR "This project cannot be built on Android.")
endif()

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/opcua/waterpump/simulationserver")

find_package(Qt6 REQUIRED COMPONENTS Core OpcUaPrivate)

qt_standard_project_setup()

if (EXISTS ../../../../../${Qt6_VERSION}/Src/qtopcua/src/plugins/opcua/open62541/qopen62541utils.cpp)
    # Installed sources
    set(MODULE_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../${Qt6_VERSION}/Src/qtopcua/)
elseif (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../../../../src/plugins/opcua/open62541/qopen62541utils.cpp)
    # Build from source tree
    set(MODULE_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/../../../../)
elseif (NOT "${QTOPCUA_SOURCE_DIR}" STREQUAL "" AND EXISTS ${QTOPCUA_SOURCE_DIR}/src/plugins/opcua/open62541/qopen62541utils.cpp)
    # Build from custom directory
    set(MODULE_SOURCE ${QTOPCUA_SOURCE_DIR})
else()
    message(FATAL_ERROR "Unable to build the server, the Qt OPC UA module source is not available."
        "Set QTOPCUA_SOURCE_DIR to the location of the Qt OPC UA sources.")
endif()

qt_add_executable(simulationserver
    ${MODULE_SOURCE}/src/plugins/opcua/open62541/qopen62541utils.cpp
    ${MODULE_SOURCE}/src/plugins/opcua/open62541/qopen62541valueconverter.cpp
    main.cpp
    simulationserver.cpp simulationserver.h
)

set_source_files_properties(${MODULE_SOURCE}/src/plugins/opcua/open62541/qopen62541valueconverter.cpp
                            PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)

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
    if(WIN32 AND (GCC OR CLANG))
        target_link_libraries(simulationserver PRIVATE
            iphlpapi
        )
    elseif(WIN32 AND MSVC)
        target_link_libraries(simulationserver PRIVATE
            iphlpapi.lib
        )
    endif()

    if (APPLE)
        set_source_files_properties(${MODULE_SOURCE}/src/3rdparty/open62541/open62541.c PROPERTIES
        COMPILE_DEFINITIONS
            "_DARWIN_C_SOURCE")
    endif()

    if (NOT (WINRT OR WIN32 AND MSVC))
        set_source_files_properties(${MODULE_SOURCE}/src/3rdparty/open62541/open62541.c
        PROPERTIES COMPILE_OPTIONS
            "-Wno-unused-parameter;-Wno-incompatible-pointer-types;-Wno-maybe-uninitialized;-Wno-format-overflow;-Wno-unused-variable;-std=c99")
    elseif(WIN32 AND MSVC)
        set_source_files_properties(${MODULE_SOURCE}/src/3rdparty/open62541/open62541.c
            PROPERTIES COMPILE_OPTIONS
                "/Zc:strictStrings-;/permissive;/wd4133")
    endif()
endif()

if (QT_FEATURE_system_open62541)
    find_package(open62541)
    if (OPEN62541_SYSTEM_ENCRYPTION)
        find_package(WrapOpenSSL)
        target_link_libraries(simulationserver PRIVATE
            WrapOpenSSL::WrapOpenSSL
        )
    endif()
    target_compile_definitions(simulationserver PRIVATE USE_SYSTEM_OPEN62541)
endif()

if(QT_FEATURE_system_open62541 OR NOT QT_FEATURE_open62541)
    target_link_libraries(simulationserver PRIVATE
        open62541::open62541
    )
endif()

install(TARGETS simulationserver
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
