# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(waterpump-qml LANGUAGES CXX)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/opcua/waterpump/waterpump-qml")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick)

qt_standard_project_setup()

qt_add_executable(waterpump-qml
    main.cpp
)

set_target_properties(waterpump-qml PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(waterpump-qml PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
)

# Resources:
set(qml_resource_files
    "MachineDisplay.qml"
    "Pump.qml"
    "ServerControl.qml"
    "Tank1Unit.qml"
    "Tank2Unit.qml"
    "TankDisplay.qml"
    "ValueDisplay.qml"
    "machine/Machine.qml"
    "machine/Tank.qml"
    "main.qml"
)

qt6_add_resources(waterpump-qml "qml"
    PREFIX
        "/"
    FILES
        ${qml_resource_files}
)

install(TARGETS waterpump-qml
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)

if (WIN32)
    target_compile_definitions(waterpump-qml PRIVATE EXAMPLES_CMAKE_SPECIFIC_PATH)
endif()
