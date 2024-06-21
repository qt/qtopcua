# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Find OpenSSL at a directory scope which is common to both the module and plugin
if (NOT CMAKE_SYSTEM_NAME STREQUAL "Windows" OR NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64"
        OR NOT CMAKE_SYSTEM_VERSION STREQUAL "10")

    set(_qt_opcua_openssl_version "3.0")

    # First make the openssl package and its targets available, without using qt_find_package.
    # This is needed because we can't set properties on not yet existing targets.
    if(NOT TARGET WrapOpenSSL::WrapOpenSSL)
        find_package(WrapOpenSSL ${_qt_opcua_openssl_version})
        set(_qt_opcua_openssl_already_found "FALSE")
    else()
        set(_qt_opcua_openssl_already_found "TRUE")
    endif()

    # Then make sure not to promote the imported targets to global when we call qt_find_package,
    # if the targets were created in the root scope when looking for the Network package
    # in a static build.
    # But if the imported targets don't exist, make sure we do promote the targets, to ensure
    # SBOM generation can access the target in the project root directory scope.
    if(_qt_opcua_openssl_already_found)
        foreach(_qt_ssl_target WrapOpenSSLHeaders::WrapOpenSSLHeaders
                               WrapOpenSSL::WrapOpenSSL)
            if(TARGET ${_qt_ssl_target})
                qt_internal_disable_find_package_global_promotion(${_qt_ssl_target})
            endif()
        endforeach()
    endif()

    # Finally call qt_find_package with PROVIDED_TARGETS to ensure the package dependency
    # is recorded for static builds.
    qt_find_package(WrapOpenSSL ${_qt_opcua_openssl_version}
                    PROVIDED_TARGETS WrapOpenSSL::WrapOpenSSL
                    MODULE_NAME network QMAKE_LIB openssl)
endif()

add_subdirectory(opcua)
add_subdirectory(plugins)
if(TARGET Qt::Quick)
    add_subdirectory(declarative_opcua)
endif()
