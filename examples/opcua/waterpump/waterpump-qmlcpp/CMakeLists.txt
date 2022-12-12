# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(waterpump-qmlcpp LANGUAGES CXX)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/opcua/waterpump/waterpump-qmlcpp")

find_package(Qt6 REQUIRED COMPONENTS Core Gui OpcUa Quick)

qt_standard_project_setup()

qt_add_executable(waterpump-qmlcpp
    main.cpp
    opcuamachinebackend.cpp opcuamachinebackend.h
)

set_target_properties(waterpump-qmlcpp PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(waterpump-qmlcpp PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::OpcUa
    Qt6::Quick
)

# Resources:
set(qml_resource_files
    "Pump.qml"
    "ServerControl.qml"
    "Tank.qml"
    "Tank1Unit.qml"
    "Tank2Unit.qml"
    "TankSimulation.qml"
    "ValueDisplay.qml"
    "main.qml"
)

qt6_add_resources(waterpump-qmlcpp "qml"
    PREFIX
        "/"
    FILES
        ${qml_resource_files}
)

install(TARGETS waterpump-qmlcpp
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)

if (WIN32)
    target_compile_definitions(waterpump-qmlcpp PRIVATE EXAMPLES_CMAKE_SPECIFIC_PATH)
endif()
