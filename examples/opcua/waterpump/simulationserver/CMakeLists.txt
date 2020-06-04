# Generated from simulationserver.pro.

cmake_minimum_required(VERSION 3.14)
project(simulationserver LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
  set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/opcua/waterpump/simulationserver")

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS OpcUa)

# special case begin
if (EXISTS ../../../../../../../${QT_REPO_MODULE_VERSION}/Automation/sources/qtopcua/)
    set(MODULE_SOURCE ../../../../../../../${QT_REPO_MODULE_VERSION}/Automation/sources/qtopcua/)
else()
    set(MODULE_SOURCE ../../../../)
endif()
# special case end

add_executable(simulationserver
    ${MODULE_SOURCE}/src/plugins/opcua/open62541/qopen62541utils.cpp # special case
    ${MODULE_SOURCE}/src/plugins/opcua/open62541/qopen62541valueconverter.cpp # special case
    main.cpp
    simulationserver.cpp simulationserver.h
)

target_include_directories(simulationserver PUBLIC
    ${MODULE_SOURCE}/src/plugins/opcua/open62541 # special case
)

target_link_libraries(simulationserver PUBLIC
    Qt::Core
    Qt::OpcUa
    Qt::OpcUaPrivate
)

if (QT_FEATURE_open62541 AND NOT QT_FEATURE_system_open62541)
    qt_extend_target(simulationserver
        SOURCES
            ${MODULE_SOURCE}/src/3rdparty/open62541/open62541.h # special case
            ${MODULE_SOURCE}/src/3rdparty/open62541/open62541.c # special case
        INCLUDE_DIRECTORIES
            ${MODULE_SOURCE}/src/3rdparty/open62541 # special case
    )
endif()

# special case begin
if (QT_FEATURE_open62541 AND NOT QT_FEATURE_system_open62541 AND NOT (WINRT OR WIN32 AND MSVC)) # special case
    set_source_files_properties(${MODULE_SOURCE}/src/3rdparty/open62541/open62541.c
        PROPERTIES COMPILE_FLAGS -Wno-unused-parameter -Wno-unused-function -Wno-format-Wno-strict-aliasing -Wno-unused-result -std=c99)
endif()
# special case end

if(QT_FEATURE_system_open62541 OR NOT QT_FEATURE_open62541)
    target_link_libraries(simulationserver PRIVATE
        open62541
    )
endif()

install(TARGETS simulationserver
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
