# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_qopcuaclient LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

#####################################################################
## tst_qopcuaclient Test:
#####################################################################

qt_internal_add_test(tst_qopcuaclient
    SOURCES
        tst_client.cpp
    LIBRARIES
        Qt::OpcUa
        Qt::OpcUaPrivate
)

# Resources:
set(data_resource_files
    "../../open62541-testserver/pki/own/certs/open62541-testserver.der"
)

qt_internal_add_resource(tst_qopcuaclient "data"
    PREFIX
        "/"
    FILES
        ${data_resource_files}
)

## Scopes:
#####################################################################

qt_internal_extend_target(tst_qopcuaclient CONDITION QT_FEATURE_open62541_security AND QT_FEATURE_open62541
    DEFINES
        SERVER_SUPPORTS_SECURITY
)

qt_internal_extend_target(tst_qopcuaclient CONDITION NOT QT_FEATURE_ns0idnames
    DEFINES
        QT_OPCUA_NO_NS0IDNAMES
)

if (WIN32)
    target_compile_definitions(tst_qopcuaclient PRIVATE TESTS_CMAKE_SPECIFIC_PATH)
endif()
