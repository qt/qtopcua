

#### Inputs

# input open62541
set(INPUT_open62541 "undefined" CACHE STRING "")
set_property(CACHE INPUT_open62541 PROPERTY STRINGS undefined no qt system)



#### Libraries



#### Tests



#### Features

qt_feature("open62541" PUBLIC PRIVATE
    LABEL "Open62541"
)
qt_feature_definition("open62541" "QT_NO_OPEN62541" NEGATE VALUE "1")
qt_feature("system-open62541" PRIVATE
    LABEL "Using provided Open62541"
    CONDITION QT_FEATURE_open62541 AND libs.open62541 OR FIXME
    ENABLE INPUT_open62541 STREQUAL 'system'
    DISABLE INPUT_open62541 STREQUAL 'qt'
)
qt_feature("uacpp" PRIVATE
    LABEL "Unified Automation C++ SDK"
    CONDITION libs.uacpp OR FIXME
)
qt_feature("mbedtls" PRIVATE
    LABEL "mbedtls"
    CONDITION libs.mbedtls OR FIXME
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
