# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

#####################################################################
## QOpen62541Plugin Plugin:
#####################################################################

qt_internal_add_plugin(QOpen62541Plugin
    OUTPUT_NAME open62541_backend
    PLUGIN_TYPE opcua
    SOURCES
        qopen62541.h
        qopen62541backend.cpp qopen62541backend.h
        qopen62541client.cpp qopen62541client.h
        qopen62541node.cpp qopen62541node.h
        qopen62541plugin.cpp qopen62541plugin.h
        qopen62541subscription.cpp qopen62541subscription.h
        qopen62541utils.cpp qopen62541utils.h
        qopen62541valueconverter.cpp qopen62541valueconverter.h
    LIBRARIES
        Qt::Core
        Qt::CorePrivate
        Qt::Network
        Qt::OpcUa
        Qt::OpcUaPrivate
)

## Scopes:
#####################################################################

qt_internal_extend_target(QOpen62541Plugin CONDITION QT_FEATURE_open62541 AND NOT QT_FEATURE_system_open62541
    SOURCES
        ../../../3rdparty/open62541/open62541.h
    ../../../3rdparty/open62541/open62541.c
    INCLUDE_DIRECTORIES
        ../../../3rdparty/open62541
    ATTRIBUTION_FILE_DIR_PATHS
        ../../../3rdparty/open62541
)

set_source_files_properties(qopen62541valueconverter.cpp
                            PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON)

#qt_internal_extend_target(QOpen62541Plugin CONDITION QT_FEATURE_mbedtls AND QT_FEATURE_open62541 AND NOT QT_FEATURE_system_open62541
#    DEFINES
#        UA_ENABLE_ENCRYPTION
#        UA_ENABLE_ENCRYPTION_MBEDTLS
#    LIBRARIES
#        mbedtls
#)
#special case end

qt_internal_extend_target(QOpen62541Plugin CONDITION QT_FEATURE_open62541_security AND WrapOpenSSL_FOUND AND QT_FEATURE_open62541 AND NOT QT_FEATURE_system_open62541
    DEFINES
        UA_ENABLE_ENCRYPTION_OPENSSL
    LIBRARIES
        WrapOpenSSL::WrapOpenSSL
)

qt_internal_extend_target(QOpen62541Plugin CONDITION QT_FEATURE_open62541_security AND WrapOpenSSL_FOUND AND QT_FEATURE_system_open62541 AND OPEN62541_SYSTEM_ENCRYPTION
    LIBRARIES
        WrapOpenSSL::WrapOpenSSL
)

qt_internal_extend_target(QOpen62541Plugin CONDITION QT_FEATURE_open62541 AND WIN32 AND MSVC AND NOT QT_FEATURE_system_open62541
    LIBRARIES
        iphlpapi.lib
)

qt_internal_extend_target(QOpen62541Plugin CONDITION QT_FEATURE_open62541 AND NOT QT_FEATURE_system_open62541 AND WIN32 AND (GCC OR CLANG)
    LIBRARIES
        iphlpapi
)

#qt_internal_extend_target(QOpen62541Plugin CONDITION QT_FEATURE_open62541 AND NOT QT_FEATURE_system_open62541 AND (WINRT OR WIN32 AND MSVC)
#    SOURCES
#        ../../../3rdparty/open62541/open62541.c
#)

if (QT_FEATURE_open62541 AND NOT QT_FEATURE_system_open62541)
    if (APPLE)
        set_source_files_properties(../../../3rdparty/open62541/open62541.c PROPERTIES
        COMPILE_DEFINITIONS
            "_DARWIN_C_SOURCE")
    endif()

    if (NOT (WINRT OR WIN32 AND MSVC))
        set_source_files_properties(../../../3rdparty/open62541/open62541.c PROPERTIES
        COMPILE_OPTIONS
            "-Wno-unused-parameter;-Wno-incompatible-pointer-types;-Wno-maybe-uninitialized;-Wno-format-overflow;-Wno-unused-variable;-std=c99")
    elseif(WIN32 AND MSVC)
        set_source_files_properties(../../../3rdparty/open62541/open62541.c PROPERTIES
            COMPILE_OPTIONS "/wd4133")
    endif()
endif()

if (QT_FEATURE_system_open62541)
    find_package(open62541 REQUIRED)
    target_compile_definitions(QOpen62541Plugin PRIVATE USE_SYSTEM_OPEN62541)
endif()

qt_internal_extend_target(QOpen62541Plugin CONDITION QT_FEATURE_system_open62541 OR NOT QT_FEATURE_open62541
    LIBRARIES
        open62541::open62541
)
