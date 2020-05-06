# Generated from opcuaviewer.pro.

cmake_minimum_required(VERSION 3.14)
project(opcuaviewer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(INSTALL_EXAMPLEDIR "examples/opcua/opcuaviewer")

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS OpcUa)
find_package(Qt6 COMPONENTS Widgets)

add_qt_gui_executable(opcuaviewer
    certificatedialog.cpp certificatedialog.h certificatedialog.ui
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    opcuamodel.cpp opcuamodel.h
    treeitem.cpp treeitem.h
)
target_link_libraries(opcuaviewer PUBLIC
    Qt::Core
    Qt::Gui
    Qt::OpcUa
    Qt::Widgets
)

install(TARGETS opcuaviewer
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
