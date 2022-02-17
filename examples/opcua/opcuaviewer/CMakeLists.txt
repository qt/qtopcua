cmake_minimum_required(VERSION 3.16)
project(opcuaviewer LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/opcua/opcuaviewer")

find_package(Qt6 REQUIRED COMPONENTS Core Gui OpcUa Widgets)

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/pki"
    DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

qt_add_executable(opcuaviewer
    certificatedialog.cpp certificatedialog.h certificatedialog.ui
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    opcuamodel.cpp opcuamodel.h
    treeitem.cpp treeitem.h
)

set_target_properties(opcuaviewer PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
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
