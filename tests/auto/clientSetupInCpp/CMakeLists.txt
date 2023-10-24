# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

#####################################################################
## tst_clientSetupInCpp Test:
#####################################################################

qt_internal_add_test(tst_clientSetupInCpp
    GUI
    QMLTEST
    QML_IMPORTPATH
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/plugins/declarative"
    SOURCES
        tst_clientSetupInCpp.cpp
    LIBRARIES
        Qt::Gui
        Qt::OpcUaPrivate
        Qt::Qml
)

## Scopes:
#####################################################################

qt_internal_extend_target(tst_clientSetupInCpp CONDITION QT_FEATURE_open62541_security
    DEFINES
        SERVER_SUPPORTS_SECURITY
)

if (WIN32)
    target_compile_definitions(tst_clientSetupInCpp PRIVATE TESTS_CMAKE_SPECIFIC_PATH)
endif()
