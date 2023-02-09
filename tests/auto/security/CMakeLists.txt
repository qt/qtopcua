# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

#####################################################################
## tst_security Test:
#####################################################################

qt_internal_add_test(tst_security
    SOURCES
        ../../common/backend_environment.h
        tst_security.cpp
    INCLUDE_DIRECTORIES
        ../../common
    LIBRARIES
        Qt::Network
        Qt::OpcUa
)

# Resources:
set(certs_resource_files
    "pki/own/certs/tst_security.der"
    "pki/own/private/privateKeyWithPassword_secret.pem"
    "pki/own/private/privateKeyWithoutPassword.pem"
    "pki/trusted/certs/ca.der"
    "pki/trusted/certs/open62541-testserver.der"
    "pki/trusted/crl/ca.crl.pem"
)

qt_internal_add_resource(tst_security "certs"
    PREFIX
        "/"
    FILES
        ${certs_resource_files}
)

if (WIN32)
    target_compile_definitions(tst_security PRIVATE TESTS_CMAKE_SPECIFIC_PATH)
endif()
