# Generated from clientSetupInCpp.pro.

#####################################################################
## tst_clientSetupInCpp Test:
#####################################################################

qt_internal_add_test(tst_clientSetupInCpp
    GUI
    QMLTEST
    QML_IMPORTPATH
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/plugins/declarative"
    SOURCES
        ../../common/backend_environment.h
        tst_clientSetupInCpp.cpp
    INCLUDE_DIRECTORIES
        ../../common
    PUBLIC_LIBRARIES
        Qt::Gui
        Qt::OpcUaPrivate
        Qt::Qml
)

#### Keys ignored in scope 1:.:.:clientSetupInCpp.pro:<TRUE>:
# IMPORTPATH = "$$PWD/../../../src/plugins/declarative"
# TEMPLATE = "app"

## Scopes:
#####################################################################

qt_internal_extend_target(tst_clientSetupInCpp CONDITION QT_FEATURE_open62541_security # special case
    DEFINES
        SERVER_SUPPORTS_SECURITY
)

# special case begin
if (WIN32)
    target_compile_definitions(tst_clientSetupInCpp PRIVATE TESTS_CMAKE_SPECIFIC_PATH)
endif()
# special case end
