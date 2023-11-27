# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# This CMake function call can be used to update the generated code
#if (QT_FEATURE_system_open62541)
# ua_generate_nodeset_and_datatypes(
#    NAME "qtopcuatestmodel"
#    FILE_NS "${CMAKE_CURRENT_SOURCE_DIR}/TestModel/qtopcuatestmodel.xml"
#    FILE_BSD "${CMAKE_CURRENT_SOURCE_DIR}/TestModel/qtopcuatestmodel.bsd"
#    FILE_CSV "${CMAKE_CURRENT_SOURCE_DIR}/TestModel/qtopcuatestmodel.csv"
#    NAMESPACE_MAP "4:http://qt-project.org/QtOpcUaTestModel/"
#    OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/generated"
# )
#endif()

#####################################################################
## open62541-testserver Binary:
#####################################################################

qt_internal_add_executable(open62541-testserver
    SOURCES
        ../../src/plugins/opcua/open62541/qopen62541utils.cpp
        ../../src/plugins/opcua/open62541/qopen62541valueconverter.cpp
        main.cpp
        testserver.cpp testserver.h
        generated/namespace_qtopcuatestmodel_generated.c
        generated/qtopcuatestmodel_nodeids.h
        generated/types_qtopcuatestmodel_generated_handling.h
        generated/namespace_qtopcuatestmodel_generated.h
        generated/types_qtopcuatestmodel_generated.c
        generated/types_qtopcuatestmodel_generated.h
    INCLUDE_DIRECTORIES
        ../../src/plugins/opcua/open62541
        generated/
    OUTPUT_DIRECTORY
        "${CMAKE_CURRENT_BINARY_DIR}"
    LIBRARIES
        Qt::Core
        Qt::OpcUaPrivate
)

set_target_properties(open62541-testserver PROPERTIES MACOSX_BUNDLE TRUE)

# Resources:
set(certs_resource_files
    "pki/own/certs/open62541-testserver.der"
    "pki/own/private/open62541-testserver.der"
    "pki/trusted/certs/opcuaviewer.der"
    "pki/trusted/certs/tst_security.der"
)

qt_internal_add_resource(open62541-testserver "certs"
    PREFIX
        "/"
    FILES
        ${certs_resource_files}
)

# We need to disable optimizations on MSVC, to circumvent buggy code in the open62541.c file.
# Otherwise the test server fails to launch on Windows MSVC with UA_STATUSCODE_BADOUTOFMEMORY.
# We also disable them on Unixy platforms, to mimic the qmake fix done in QTBUG-75020.
# See QTBUG-85939 for details.
qt_opcua_disable_optimizations_in_current_dir()

## Scopes:
#####################################################################

qt_internal_extend_target(open62541-testserver CONDITION QT_FEATURE_open62541 AND NOT QT_FEATURE_system_open62541
    SOURCES
        ../../src/3rdparty/open62541/open62541.h
    ../../src/3rdparty/open62541/open62541.c
    INCLUDE_DIRECTORIES
        ../../src/3rdparty/open62541
    DEFINES
        UA_ENABLE_AMALGAMATION
)

#qt_internal_extend_target(open62541-testserver CONDITION QT_FEATURE_mbedtls AND QT_FEATURE_open62541 AND NOT QT_FEATURE_system_open62541
#    DEFINES
#        UA_ENABLE_ENCRYPTION
#        UA_ENABLE_ENCRYPTION_MBEDTLS
#    LIBRARIES
#        mbedtls
#)

if (QT_FEATURE_open62541_security AND NOT QT_FEATURE_system_open62541)
    find_package(WrapOpenSSL 3.0)
endif()

qt_internal_extend_target(open62541-testserver CONDITION QT_FEATURE_open62541_security AND WrapOpenSSL_FOUND AND QT_FEATURE_open62541 AND NOT QT_FEATURE_system_open62541
    DEFINES
        UA_ENABLE_ENCRYPTION_OPENSSL
    LIBRARIES
        WrapOpenSSL::WrapOpenSSL
)

qt_internal_extend_target(open62541-testserver CONDITION QT_FEATURE_open62541 AND WIN32 AND MSVC AND NOT QT_FEATURE_system_open62541
    LIBRARIES
        ws2_32.lib
)

qt_internal_extend_target(open62541-testserver CONDITION (QT_FEATURE_open62541 AND NOT QT_FEATURE_system_open62541) AND (WIN32 AND GCC)
    LIBRARIES
        ws2_32
)

#qt_internal_extend_target(open62541-testserver CONDITION QT_FEATURE_open62541 AND NOT QT_FEATURE_system_open62541 AND (WINRT OR WIN32 AND MSVC)
#    SOURCES
#        ../../src/3rdparty/open62541/open62541.c
#)

if (QT_FEATURE_open62541 AND NOT QT_FEATURE_system_open62541)
    if (NOT (WINRT OR WIN32 AND MSVC))
        set_source_files_properties(../../src/3rdparty/open62541/open62541.c PROPERTIES
        COMPILE_OPTIONS
            "-Wno-unused-parameter;-Wno-unused-function;-Wno-format;-Wno-format-security;-Wno-strict-aliasing;-Wno-unused-result;-std=c99")
    elseif(WIN32 AND MSVC)
        # CMake uses C11+ by default and the _Static_assert expansion fails during compilation
        set_source_files_properties(../../src/3rdparty/open62541/open62541.c PROPERTIES LANGUAGE CXX)
        set_source_files_properties(../../src/3rdparty/open62541/open62541.c PROPERTIES
        COMPILE_OPTIONS
            "/Zc:strictStrings-;/permissive;/wd4200")
    endif()
endif()

qt_internal_extend_target(open62541-testserver CONDITION QT_FEATURE_system_open62541 OR NOT QT_FEATURE_open62541
    LIBRARIES
        open62541
)
