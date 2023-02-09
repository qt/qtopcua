# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

#####################################################################
## tst_qopcuaclient Test:
#####################################################################

qt_internal_add_test(tst_qopcuaclient
    SOURCES
        ../../common/backend_environment.h
        tst_client.cpp
    INCLUDE_DIRECTORIES
        ../../common
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

if (WIN32)
    target_compile_definitions(tst_qopcuaclient PRIVATE TESTS_CMAKE_SPECIFIC_PATH)
endif()
