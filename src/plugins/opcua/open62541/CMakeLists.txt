# Generated from open62541.pro.

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

#### Keys ignored in scope 1:.:.:open62541.pro:<TRUE>:
# OTHER_FILES = "open62541-metadata.json"

## Scopes:
#####################################################################

qt_internal_extend_target(QOpen62541Plugin CONDITION QT_FEATURE_open62541 AND NOT QT_FEATURE_system_open62541
    SOURCES
        ../../../3rdparty/open62541/open62541.h
# special case begin
    ../../../3rdparty/open62541/open62541.c
# special case end
    INCLUDE_DIRECTORIES
        ../../../3rdparty/open62541
)

# special case begin
#qt_internal_extend_target(QOpen62541Plugin CONDITION QT_FEATURE_mbedtls AND QT_FEATURE_open62541 AND NOT QT_FEATURE_system_open62541
#    DEFINES
#        UA_ENABLE_ENCRYPTION
#        UA_ENABLE_ENCRYPTION_MBEDTLS
#    LIBRARIES
#        mbedtls
#)
#special case end

# special case begin
qt_internal_extend_target(QOpen62541Plugin CONDITION QT_FEATURE_open62541_security AND WrapOpenSSL_FOUND AND QT_FEATURE_open62541 AND NOT QT_FEATURE_system_open62541
    DEFINES
        UA_ENABLE_ENCRYPTION
        UA_ENABLE_ENCRYPTION_OPENSSL
    LIBRARIES
        WrapOpenSSL::WrapOpenSSL
)
# special case end

qt_internal_extend_target(QOpen62541Plugin CONDITION QT_FEATURE_open62541 AND WIN32 AND MSVC AND NOT QT_FEATURE_system_open62541 # special case
    LIBRARIES
        ws2_32.lib
)

qt_internal_extend_target(QOpen62541Plugin CONDITION (QT_FEATURE_open62541 AND NOT QT_FEATURE_system_open62541) AND (WIN32 AND GCC) # special case
    LIBRARIES
        ws2_32
)

# special case begin
#qt_internal_extend_target(QOpen62541Plugin CONDITION QT_FEATURE_open62541 AND NOT QT_FEATURE_system_open62541 AND (WINRT OR WIN32 AND MSVC)
#    SOURCES
#        ../../../3rdparty/open62541/open62541.c
#)
# special case end

# special case begin
if (QT_FEATURE_open62541 AND NOT QT_FEATURE_system_open62541)
    if (NOT (WINRT OR WIN32 AND MSVC)) # special case
        set_source_files_properties(../../../3rdparty/open62541/open62541.c PROPERTIES
        COMPILE_OPTIONS
            "-Wno-unused-parameter;-Wno-unused-function;-Wno-format;-Wno-format-security;-Wno-strict-aliasing;-Wno-unused-result;-std=c99")
    elseif(WIN32 AND MSVC)
        # CMake uses C11+ by default and the _Static_assert expansion fails during compilation
        set_source_files_properties(../../../3rdparty/open62541/open62541.c PROPERTIES
            LANGUAGE CXX
            SKIP_PRECOMPILE_HEADERS ON
            COMPILE_OPTIONS "/Zc:strictStrings-;/permissive;/wd2362")
    endif()
endif()
# special case end

#### Keys ignored in scope 10:.:../../../3rdparty:../../../3rdparty/open62541.pri:else:
# OPEN62541_CFLAGS = "-Wno-unused-parameter" "-Wno-unused-function" "-Wno-format" "-Wno-strict-aliasing" "-Wno-unused-result" "-std=c99"
# QMAKE_EXTRA_COMPILERS = "open62541_compiler"
# open62541_compiler.commands = "$$QMAKE_CC" "-c" "$(CFLAGS)" "$$OPEN62541_CFLAGS" "$(INCPATH)" "${QMAKE_FILE_IN}" "-o" "${QMAKE_FILE_OUT}"
# open62541_compiler.dependency_type = "TYPE_C"
# open62541_compiler.input = "OPEN62541_SOURCES"
# open62541_compiler.name = "compiling" "${QMAKE_FILE_IN}"
# open62541_compiler.output = "${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_BASE}$${firstQMAKE_EXT_OBJ}"

#### Keys ignored in scope 11:.:../../../3rdparty:../../../3rdparty/open62541.pri:silent:
# open62541_compiler.commands = "@echo" "compiling" "${QMAKE_FILE_IN}" "&&"

qt_internal_extend_target(QOpen62541Plugin CONDITION QT_FEATURE_system_open62541 OR NOT QT_FEATURE_open62541
    LIBRARIES
        open62541
)

# special case begin
#qt_internal_extend_target(QOpen62541Plugin CONDITION WIN32 AND MSVC AND (QT_FEATURE_system_open62541 OR NOT QT_FEATURE_open62541)
#    PUBLIC_LIBRARIES
#        open62541.lib
#)
# special case end
