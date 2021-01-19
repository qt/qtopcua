# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause



#### Inputs

# input open62541
set(INPUT_open62541 "undefined" CACHE STRING "")
set_property(CACHE INPUT_open62541 PROPERTY STRINGS undefined no qt system)



#### Libraries
# special case begin
if (INPUT_open62541 STREQUAL "system")
    qt_find_package(Open62541 PROVIDED_TARGETS open62541)
endif()
qt_find_package(Uacpp PROVIDED_TARGETS uacpp)
# special case end

#### Tests

# special case begin
if (INPUT_open62541 STREQUAL "system")
    qt_config_compile_test(open62541
        LABEL "open62541 support"
        LIBRARIES
            open62541
        CODE
    "#include <stdio.h>
    #include <open62541.h>

    int main(int argc, char *argv[])
    {
        /* Create a client and connect */
        UA_Client *client = UA_Client_new();
        UA_ClientConfig_setDefault(UA_Client_getConfig(client));
        UA_StatusCode status = UA_Client_connect(client, \"opc.tcp://localhost:4840\");
        if (status != UA_STATUSCODE_GOOD) {
            UA_Client_delete(client);
            return status;
        }

        /* Read the value attribute of the node. UA_Client_readValueAttribute is a
         * wrapper for the raw read service available as UA_Client_Service_read. */
        UA_Variant value; /* Variants can hold scalar values and arrays of any type */
        UA_Variant_init(&value);
        status = UA_Client_readValueAttribute(client, UA_NODEID_STRING(1, (char*)(\"the.answer\")), &value);
        if (status == UA_STATUSCODE_GOOD &&
           UA_Variant_hasScalarType(&value, &UA_TYPES[UA_TYPES_INT32])) {
            printf(\"the value is: %i\\\\n\", *(UA_Int32*)value.data);
        }

        /* Clean up */
        UA_Variant_clear(&value);
        UA_Client_delete(client); /* Disconnects the client internally */
        return status;
    }")
endif()
# special case end

# special case begin
qt_config_compile_test(uacpp
    LABEL "uacpp support"
    LIBRARIES
        uacpp
    CODE
"#include <stdio.h>

#include <uaplatformlayer.h>
#include <uastring.h>
#include <uasession.h>

using namespace UaClientSdk;

int main(int /*argc*/, char ** /*argv*/)
{

    UaPlatformLayer::init();
    UaSession *session = new UaSession;

    UaPlatformLayer::cleanup();
    return 0;
}")
# special case end

# Find out if the system open62541 is built with encryption support
include(CheckSymbolExists)
if (Open62541_FOUND)
    check_symbol_exists(UA_ENABLE_ENCRYPTION ${Open62541_INCLUDE_DIRS}/open62541.h OPEN62541_SYSTEM_ENCRYPTION)
endif()

#### Features

qt_feature("open62541" PUBLIC PRIVATE
    LABEL "Open62541"
# special case begin
    ENABLE INPUT_open62541 STREQUAL 'qt' OR QT_FEATURE_system_open62541
    DISABLE INPUT_open62541 STREQUAL 'no' OR (NOT QT_FEATURE_system_open62541 AND INPUT_open62541 STREQUAL 'system')
# special case end
)
qt_feature_definition("open62541" "QT_NO_OPEN62541" NEGATE VALUE "1")
qt_feature("system-open62541" PRIVATE
    LABEL "Using provided Open62541"
    CONDITION Open62541_FOUND AND TEST_open62541 # special case
    ENABLE INPUT_open62541 STREQUAL 'system'
    DISABLE INPUT_open62541 STREQUAL 'qt' OR INPUT_open62541 STREQUAL 'no' # special case
)
qt_feature("uacpp" PRIVATE
    LABEL "Unified Automation C++ SDK"
    CONDITION TEST_uacpp # special case
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
qt_feature("ns0idgenerator" PRIVATE
    LABEL "Namespace 0 NodeIds generator"
    PURPOSE "Build a generator for updating the QOpcUa::NodeIds::Namespace0 enum from the NodeIds.csv file."
    AUTODETECT false
)
qt_feature("gds" PUBLIC PRIVATE
    LABEL "Support for global discovery server"
    PURPOSE "Enables QOpcUaClient to interact with a global discovery server"
    CONDITION WrapOpenSSL_FOUND # special case
)
# special case begin
qt_feature("open62541-security" PUBLIC PRIVATE
    LABEL "Open62541 security support"
    PURPOSE "Enables the open62541 plugin to connect to servers with signing and encryption"
    CONDITION (WrapOpenSSL_FOUND AND QT_FEATURE_open62541 AND NOT QT_FEATURE_system_open62541) OR OPEN62541_SYSTEM_ENCRYPTION
)
# special case end
qt_feature_definition("gds" "QT_NO_GDS" NEGATE VALUE "1")
qt_configure_add_summary_section(NAME "Qt Opcua")
qt_configure_add_summary_entry(ARGS "open62541")
qt_configure_add_summary_entry(ARGS "uacpp")
qt_configure_add_summary_entry(ARGS "ns0idnames")
qt_configure_add_summary_entry(ARGS "ns0idgenerator")
qt_configure_add_summary_entry(ARGS "open62541-security") # special case
qt_configure_add_summary_entry(ARGS "gds")
qt_configure_end_summary_section() # end of "Qt Opcua" section
