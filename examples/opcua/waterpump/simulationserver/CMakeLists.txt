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

find_package(Qt6 REQUIRED COMPONENTS Core OpcUa)

qt_standard_project_setup()

if(NOT QTOPCUA_OPEN62541_SOURCE_DIR)
    find_package(open62541 QUIET)
endif()

if(NOT TARGET open62541::open62541)
    if(NOT QTOPCUA_OPEN62541_SOURCE_DIR)
        # Defaults for builds from the source tree or next to an installed Qt
        foreach(candidate IN ITEMS
                "${CMAKE_CURRENT_SOURCE_DIR}/../../../../src/3rdparty/open62541"
                "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../${Qt6_VERSION}/Src/qtopcua/src/3rdparty/open62541")
            if(EXISTS "${candidate}/open62541.c")
                set(QTOPCUA_OPEN62541_SOURCE_DIR "${candidate}")
                break()
            endif()
        endforeach()
    endif()

    if(NOT EXISTS "${QTOPCUA_OPEN62541_SOURCE_DIR}/open62541.c")
        message(FATAL_ERROR "Unable to build the server, open62541 was not found. "
            "Either install the open62541 system library, or set QTOPCUA_OPEN62541_SOURCE_DIR "
            "to the directory containing open62541.c (src/3rdparty/open62541 in the Qt OPC UA "
            "sources).")
    endif()
endif()

qt_add_executable(simulationserver
    main.cpp
    simulationserver.cpp simulationserver.h
)

set_target_properties(simulationserver PROPERTIES
    WIN32_EXECUTABLE FALSE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(simulationserver PRIVATE
    Qt6::Core
    Qt6::OpcUa
)

if(TARGET open62541::open62541)
    target_compile_definitions(simulationserver PRIVATE USE_SYSTEM_OPEN62541)
    target_link_libraries(simulationserver PRIVATE open62541::open62541)

    # Qt links OpenSSL itself if the system open62541 was built with encryption support
    include(CheckSymbolExists)
    get_target_property(open62541_incdir open62541::open62541 INTERFACE_INCLUDE_DIRECTORIES)
    check_symbol_exists(UA_ENABLE_ENCRYPTION_OPENSSL "${open62541_incdir}/open62541/config.h"
                        OPEN62541_SYSTEM_ENCRYPTION)
    if(OPEN62541_SYSTEM_ENCRYPTION)
        find_package(WrapOpenSSL)
        target_link_libraries(simulationserver PRIVATE WrapOpenSSL::WrapOpenSSL)
    endif()
else()
    target_sources(simulationserver PRIVATE
        ${QTOPCUA_OPEN62541_SOURCE_DIR}/open62541.h
        ${QTOPCUA_OPEN62541_SOURCE_DIR}/open62541.c
    )
    target_include_directories(simulationserver SYSTEM PRIVATE
        ${QTOPCUA_OPEN62541_SOURCE_DIR}
    )

    if(WIN32 AND
            (CMAKE_CXX_COMPILER_ID MATCHES "GNU"
            OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        )
        target_link_libraries(simulationserver PRIVATE
            iphlpapi
        )
    elseif(WIN32 AND MSVC)
        target_link_libraries(simulationserver PRIVATE
            iphlpapi.lib
        )
    endif()

    if (APPLE)
        set_source_files_properties(${QTOPCUA_OPEN62541_SOURCE_DIR}/open62541.c PROPERTIES
        COMPILE_DEFINITIONS
            "_DARWIN_C_SOURCE")
    endif()

    if (NOT (WINRT OR WIN32 AND MSVC))
        set_source_files_properties(${QTOPCUA_OPEN62541_SOURCE_DIR}/open62541.c
        PROPERTIES COMPILE_OPTIONS
            "-Wno-unused-parameter;-Wno-uninitialized;-Wno-unused-function;-Wno-unused-variable;-std=c99")
    elseif(WIN32 AND MSVC)
        set_source_files_properties(${QTOPCUA_OPEN62541_SOURCE_DIR}/open62541.c
            PROPERTIES COMPILE_OPTIONS
                "/Zc:strictStrings-;/permissive;/wd4133")
    endif()
endif()

install(TARGETS simulationserver
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
