# Generated from declarative.pro.

#####################################################################
## tst_opcua Test:
#####################################################################

qt_internal_add_test(tst_opcua
    GUI
    QMLTEST
    QML_IMPORTPATH
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/plugins/declarative"
    SOURCES
        ../../common/backend_environment.h
        tst_opcua.cpp
    INCLUDE_DIRECTORIES
        ../../common
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::OpcUaPrivate
        Qt::Qml
)

#### Keys ignored in scope 1:.:.:declarative.pro:<TRUE>:
# IMPORTPATH = "$$PWD/../../../src/plugins/declarative"
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_internal_extend_target(tst_opcua CONDITION QT_FEATURE_open62541_security # special case
    DEFINES
        SERVER_SUPPORTS_SECURITY
)

# special case begin
if (WIN32)
    target_compile_definitions(tst_opcua PRIVATE TESTS_CMAKE_SPECIFIC_PATH)
endif()
# special case end
