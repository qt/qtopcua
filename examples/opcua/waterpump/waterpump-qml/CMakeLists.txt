# Generated from waterpump-qml.pro.

cmake_minimum_required(VERSION 3.14)
project(waterpump-qml LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(INSTALL_EXAMPLEDIR "examples/opcua/waterpump/waterpump-qml")

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Quick)

add_qt_gui_executable(waterpump-qml
    main.cpp
)
target_link_libraries(waterpump-qml PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Quick
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
