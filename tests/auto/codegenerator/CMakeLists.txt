# Copyright (C) 2024 basysKom GmbH, opensource@basyskom.com
# SPDX-License-Identifier: BSD-3-Clause

if(NOT QT_BUILD_STANDALONE_TESTS AND NOT QT_BUILDING_QT)
    cmake_minimum_required(VERSION 3.16)
    project(tst_codegen LANGUAGES CXX)
    find_package(Qt6BuildInternals REQUIRED COMPONENTS STANDALONE_TEST)
endif()

#####################################################################
## tst_qopcuaclient Test:
#####################################################################

qt_internal_add_test(tst_codegen
    SOURCES
        tst_codegen.cpp
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_BINARY_DIR}
    LIBRARIES
        Qt::OpcUa
        Qt::OpcUaPrivate
)

qt_opcua_generate_datatypes(
    tst_codegen
    OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated"
    INPUT_BSD "${CMAKE_CURRENT_SOURCE_DIR}/../../open62541-testserver/TestModel/qtopcuatestmodel.bsd"
    INPUT_CSV_MAP "TestModel:${CMAKE_CURRENT_SOURCE_DIR}/../../open62541-testserver/TestModel/qtopcuatestmodel.csv"
    PREFIX "CodegenTest"
)

qt_opcua_generate_datatypes(
    tst_codegen
    OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated2"
    INPUT_CSV_MAP "CsvOnlyTest:${CMAKE_CURRENT_SOURCE_DIR}/testids.csv"
    PREFIX "CsvOnlyTest"
)

if (WIN32)
    target_compile_definitions(tst_codegen PRIVATE TESTS_CMAKE_SPECIFIC_PATH)
endif()
