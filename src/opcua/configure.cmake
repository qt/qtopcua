

#### Inputs

# input open62541
set(INPUT_open62541 "undefined" CACHE STRING "")
set_property(CACHE INPUT_open62541 PROPERTY STRINGS undefined no qt system)

#### Libraries

if (INPUT_open62541 STREQUAL "system")
    qt_find_package(Open62541 PROVIDED_TARGETS open62541)
endif()
qt_find_package(Mbedtls PROVIDED_TARGETS mbedtls)
qt_find_package(Uacpp PROVIDED_TARGETS uacpp)

#### Tests

qt_config_compile_test(mbedtls
    LABEL "mbedTLS support"
    LIBRARIES
        mbedtls
    CODE
"#include <mbedtls/x509.h>
#include <mbedtls/x509_crt.h>

int main(int argc, char *argv[])
{
    mbedtls_pk_context pk;
    mbedtls_pk_init( &pk );

    mbedtls_x509_crt remoteCertificate;
    mbedtls_x509_crt_init(&remoteCertificate);

    return 0;
}
")

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
        UA_Variant_deleteMembers(&value);
        UA_Client_delete(client); /* Disconnects the client internally */
        return status;
    }")
endif()

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


#### Features

qt_feature("open62541" PUBLIC PRIVATE
    LABEL "Open62541"
    ENABLE INPUT_open62541 STREQUAL 'qt' OR QT_FEATURE_system_open62541
    DISABLE INPUT_open62541 STREQUAL 'no' OR (NOT QT_FEATURE_system_open62541 AND INPUT_open62541 STREQUAL 'system')
)
qt_feature_definition("open62541" "QT_NO_OPEN62541" NEGATE VALUE "1")
qt_feature("system-open62541" PRIVATE
    LABEL "Using provided Open62541"
    CONDITION Open62541_FOUND AND TEST_open62541
    ENABLE INPUT_open62541 STREQUAL 'system'
    DISABLE INPUT_open62541 STREQUAL 'qt' OR INPUT_open62541 STREQUAL 'no'
)
qt_feature("uacpp" PRIVATE
    LABEL "Unified Automation C++ SDK"
    CONDITION TEST_uacpp
)
qt_feature("mbedtls" PRIVATE
    LABEL "mbedtls"
    CONDITION Mbedtls_FOUND AND TEST_mbedtls
)
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
)
qt_feature_definition("gds" "QT_NO_GDS" NEGATE VALUE "1")
qt_configure_add_summary_section(NAME "Qt Opcua")
qt_configure_add_summary_entry(ARGS "open62541")
qt_configure_add_summary_entry(ARGS "uacpp")
qt_configure_add_summary_entry(ARGS "ns0idnames")
qt_configure_add_summary_entry(ARGS "ns0idgenerator")
qt_configure_add_summary_entry(ARGS "mbedtls")
qt_configure_add_summary_entry(ARGS "gds")
qt_configure_end_summary_section() # end of "Qt Opcua" section
