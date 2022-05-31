# Generated from src.pro.

# Find OpenSSL at a directory scope which is common to both the module and plugin
if (NOT CMAKE_SYSTEM_NAME STREQUAL "Windows" OR NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64"
        OR NOT CMAKE_SYSTEM_VERSION STREQUAL "10")
    qt_find_package(WrapOpenSSL 1.1 PROVIDED_TARGETS WrapOpenSSL::WrapOpenSSL
                    MODULE_NAME network QMAKE_LIB openssl)
endif()

add_subdirectory(opcua)
add_subdirectory(plugins)
if(TARGET Qt::Quick)
    add_subdirectory(imports)
endif()
