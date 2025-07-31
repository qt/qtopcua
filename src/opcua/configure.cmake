# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause



#### Inputs

# input open62541
set(INPUT_open62541 "undefined" CACHE STRING "")
set_property(CACHE INPUT_open62541 PROPERTY STRINGS undefined qt system)



#### Libraries
# special case begin
if (INPUT_open62541 STREQUAL "system")
    qt_find_package(open62541 PROVIDED_TARGETS open62541)
endif()
# special case end

# Find out if the system open62541 is built with encryption support
include(CheckSymbolExists)
if (TARGET open62541::open62541)
    get_target_property(OPEN62541_INCDIR open62541::open62541 INTERFACE_INCLUDE_DIRECTORIES)
    check_symbol_exists(UA_ENABLE_ENCRYPTION_OPENSSL ${OPEN62541_INCDIR}/open62541/config.h OPEN62541_SYSTEM_ENCRYPTION)
endif()

#### Features

qt_feature("open62541" PUBLIC PRIVATE
    LABEL "Open62541"
# special case begin
    ENABLE INPUT_open62541 STREQUAL 'qt' OR QT_FEATURE_system_open62541
    DISABLE NOT QT_FEATURE_system_open62541 AND INPUT_open62541 STREQUAL 'system'
# special case end
)
qt_feature_definition("open62541" "QT_NO_OPEN62541" NEGATE VALUE "1")
qt_feature("system-open62541" PRIVATE SYSTEM_LIBRARY
    LABEL "Using provided Open62541"
    CONDITION open62541_FOUND
    ENABLE INPUT_open62541 STREQUAL 'system'
    DISABLE INPUT_open62541 STREQUAL 'qt' OR INPUT_open62541 STREQUAL 'no' # special case
)
# special case begin
#qt_feature("mbedtls" PRIVATE
#    LABEL "mbedtls"
#    CONDITION Mbedtls_FOUND AND TEST_mbedtls # special case
#)
# special case end
qt_feature("ns0idnames" PRIVATE
    LABEL "Support for namespace 0 NodeId names"
    PURPOSE "Provides names for the QOpcUa::NodeIds::Namespace0 enum."
)
qt_feature("internalgenerator" PRIVATE
    LABEL "Internal code generator"
    PURPOSE "Build a generator for updating the status codes and namespace 0 ids from the official schema files."
    AUTODETECT false
)
qt_feature("gds" PUBLIC PRIVATE
    LABEL "Support for global discovery server (requires OpenSSL 3.x)"
    PURPOSE "Enables QOpcUaClient to interact with a global discovery server"
    CONDITION WrapOpenSSL_FOUND AND QT_FEATURE_opensslv30
)
# special case begin
qt_feature("open62541-security" PUBLIC PRIVATE
    LABEL "Open62541 security support"
    PURPOSE "Enables the open62541 plugin to connect to servers with signing and encryption"
    CONDITION (WrapOpenSSL_FOUND AND QT_FEATURE_open62541 AND NOT QT_FEATURE_system_open62541) OR OPEN62541_SYSTEM_ENCRYPTION
)
# special case end
qt_feature("datatypecodegenerator" PRIVATE
    LABEL "Data type code generator"
    PURPOSE "Build a generator for generating de- and encodable data types from a .bsd file."
)
qt_feature_definition("gds" "QT_NO_GDS" NEGATE VALUE "1")
qt_configure_add_summary_section(NAME "Qt Opcua")
qt_configure_add_summary_entry(ARGS "open62541")
qt_configure_add_summary_entry(ARGS "ns0idnames")
qt_configure_add_summary_entry(ARGS "internalgenerator")
qt_configure_add_summary_entry(ARGS "open62541-security") # special case
qt_configure_add_summary_entry(ARGS "gds")
qt_configure_add_summary_entry(ARGS "datatypecodegenerator")
qt_configure_end_summary_section() # end of "Qt Opcua" section
