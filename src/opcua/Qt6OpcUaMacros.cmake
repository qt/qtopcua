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
