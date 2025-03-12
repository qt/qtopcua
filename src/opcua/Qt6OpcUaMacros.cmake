# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Removes optimizations flags in the called directory scope.
function(qt_opcua_disable_optimizations_in_current_dir)
    if(CMAKE_CONFIGURATION_TYPES)
        set(configs ${CMAKE_CONFIGURATION_TYPES})
    elseif(CMAKE_BUILD_TYPE)
        set(configs ${CMAKE_BUILD_TYPE})
    else()
        message(WARNING "No value set for CMAKE_BUILD_TYPE. Can't disable optimizations.")
        return()
    endif()
    foreach(config ${configs})
        string(TOUPPER "${config}" config_upper_case)
        set(flags_var_name "CMAKE_C_FLAGS_${config_upper_case}")

        if(MSVC)
            set(optimize_flags_to_replace "/O1" "/O2" "/O3")
            set(flag_replacement "/O0")
        else()
            set(optimize_flags_to_replace "-O1" "-O2" "-O3")
            set(flag_replacement "-O0")
        endif()
        foreach(flag_to_replace ${optimize_flags_to_replace})
            string(REPLACE
                   "${flag_to_replace}" "${flag_replacement}"
                   "${flags_var_name}" "${${flags_var_name}}")
        endforeach()
    endforeach()
    set("${flags_var_name}" "${${flags_var_name}}" PARENT_SCOPE)
endfunction()

# Generates code for OPC UA data types and/or node ids using the qopcuaxmldatatypes2cpp tool
function(qt_opcua_generate_datatypes target)
    if (NOT QT_FEATURE_datatypecodegenerator)
        message(FATAL_ERROR "Qt OPC UA was built without FEATURE_datatypecodegenerator, unable to generate code")
    endif()

    if(NOT TARGET "${target}")
        message(FATAL_ERROR "\"${target}\" is not a target.")
    endif()

    set(options BUILTIN INTERNAL "")
    set(one_value_args OUTPUT_DIR PREFIX)
    set(multi_value_args INPUT_BSD INPUT_CSV_MAP DEPENDENCY_BSD)
    cmake_parse_arguments(GEN_OPT "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN} )

    if("${GEN_OPT_INPUT_BSD}" STREQUAL "" AND "${GEN_OPT_INPUT_CSV_MAP}" STREQUAL "")
        message(FATAL_ERROR "At least one INPUT_BSD OR INPUT_CSV_MAP value must be specified for data type code generation")
    endif()

    if(NOT GEN_OPT_OUTPUT_DIR OR "${GEN_OPT_OUTPUT_DIR}" STREQUAL "")
        message(FATAL_ERROR "OUTPUT_DIR must be specified for data type code generation")
    endif()

    if(NOT GEN_OPT_PREFIX OR "${GEN_OPT_PREFIX}" STREQUAL "")
        message(FATAL_ERROR "PREFIX must be specified for data type code generation")
    endif()

    set(INPUT_BSD_ARGS "")
    foreach(f ${GEN_OPT_INPUT_BSD})
        set(INPUT_BSD_ARGS ${INPUT_BSD_ARGS} "-i=${f}")
    endforeach()

    set(INPUT_CSV_FILES "")
    set(INPUT_CSV_ARGS "")
    foreach(f ${GEN_OPT_INPUT_CSV_MAP})
        set(INPUT_CSV_ARGS ${INPUT_CSV_ARGS} "-n=${f}")
        string(FIND "${f}" ":" SEPARATOR_INDEX)
        if ("${SEPARATOR_INDEX}" STREQUAL "-1" OR "${SEPARATOR_INDEX}" STREQUAL "0")
            message(FATAL_ERROR "Invalid CSV input: ${f}: Name:Path expected")
        endif()

        MATH(EXPR PATH_START "${SEPARATOR_INDEX}+1")
        string(SUBSTRING ${f} ${PATH_START} -1 CSV_PATH)
        if ("${CSV_PATH}" STREQUAL "")
            message(FATAL_ERROR "Empty CSV input file for ${f}")
        endif()
        list(APPEND INPUT_CSV_FILES ${CSV_PATH})
    endforeach()

    set(DEPENDENCY_BSD_ARGS "")
    foreach(f ${GEN_OPT_DEPENDENCY_BSD})
        set(DEPENDENCY_BSD_ARGS ${DEPENDENCY_BSD_ARGS} "-d=${f}")
    endforeach()

    string(TOLOWER ${GEN_OPT_PREFIX} LOWER_PREFIX)

    set(GENERATED_NODEIDS_HEADER "")
    if (NOT "${GEN_OPT_INPUT_CSV_MAP}" STREQUAL "")
        set(GENERATED_NODEIDS_HEADER "${GEN_OPT_OUTPUT_DIR}/${LOWER_PREFIX}nodeids.h")
    endif()
    set(GENERATED_DATATYPES_HEADERS "")
    set(GENERATED_DATATYPES_SOURCES "")
    if (NOT "${GEN_OPT_INPUT_BSD}" STREQUAL "")
        set(GENERATED_DATATYPES_HEADERS "${GEN_OPT_OUTPUT_DIR}/${LOWER_PREFIX}binarydeencoder.h;${GEN_OPT_OUTPUT_DIR}/${LOWER_PREFIX}datatypes.h")
        set(GENERATED_DATATYPES_SOURCES "${GEN_OPT_OUTPUT_DIR}/${LOWER_PREFIX}binarydeencoder.cpp;${GEN_OPT_OUTPUT_DIR}/${LOWER_PREFIX}datatypes.cpp")
    endif()

    message("qopcuaxmldatatypes2cpp: Generated files for ${GEN_OPT_PREFIX} will be written to ${GEN_OPT_OUTPUT_DIR} and were added to target ${target}")

    add_custom_command(
        OUTPUT
            ${GENERATED_NODEIDS_HEADER}
            ${GENERATED_DATATYPES_HEADERS}
            ${GENERATED_DATATYPES_SOURCES}
        COMMAND ${QT_CMAKE_EXPORT_NAMESPACE}::qopcuaxmldatatypes2cpp -b ${INPUT_BSD_ARGS} ${DEPENDENCY_BSD_ARGS} ${INPUT_CSV_ARGS} -p ${GEN_OPT_PREFIX} -o ${GEN_OPT_OUTPUT_DIR}
        DEPENDS ${GEN_OPT_INPUT_BSD} ${GEN_OPT_DEPENDENCY_BSD} ${INPUT_CSV_FILES}
    )

    target_sources(${target} PRIVATE
        ${GENERATED_NODEIDS_HEADER}
        ${GENERATED_DATATYPES_HEADERS}
        ${GENERATED_DATATYPES_SOURCES}
    )
endfunction()
