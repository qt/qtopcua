#.rst:
# FindUacpp
# ---------
#
# Try to locate the uacpp library.
# If found, this will define the following variables:
#
# ``Uacpp_FOUND``
#     True if the mbedtls library is available
# ``Uacpp_INCLUDE_DIRS``
#     The mbedTLS include directories
# ``Mbedx509_LIBRARIES``
#     The mbedx509 library for linking
# ``Mbedcrypto_LIBRARIES``
#     The mbedcrypto library for linking
#
# If ``Uacpp_FOUND`` is TRUE, it will also define the following
# imported targets:
#
# ``uacpp``
#     An interface library combining the uacpp libraries

find_path(Uabasecpp_INCLUDE_DIRS
    NAMES uaplatformlayer.h
    HINTS "${UACPP_PREFIX}/include"
    PATH_SUFFIXES "uabasecpp")

find_path(Uastack_INCLUDE_DIRS
    NAMES opcua_platformdefs.h
    HINTS "${UACPP_PREFIX}/include"
    PATH_SUFFIXES "uastack")

find_path(Uaclientcpp_INCLUDE_DIRS
    NAMES uaclientsdk.h
    HINTS "${UACPP_PREFIX}/include"
    PATH_SUFFIXES "uaclientcpp")

find_path(Uapkicpp_INCLUDE_DIRS
    NAMES uapkicertificate.h
    HINTS "${UACPP_PREFIX}/include"
    PATH_SUFFIXES "uapkicpp")

find_path(Xmlparsercpp_INCLUDE_DIRS
    NAMES opcua_xml_parser.h
    HINTS "${UACPP_PREFIX}/include"
    PATH_SUFFIXES "xmlparsercpp")

# Use the release or debug libs depending on the build type
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    find_library(Uaclientcpp_LIBRARIES
        NAMES uaclientcppd
        HINTS "${UACPP_PREFIX}/lib")

    find_library(Uabasecpp_LIBRARIES
        NAMES uabasecppd
        HINTS "${UACPP_PREFIX}/lib")

    find_library(Uastack_LIBRARIES
        NAMES uastackd
        HINTS "${UACPP_PREFIX}/lib")

    find_library(Uapkicpp_LIBRARIES
        NAMES uapkicppd
        HINTS "${UACPP_PREFIX}/lib")

    find_library(Xmlparsercpp_LIBRARIES
        NAMES xmlparsercppd
        HINTS "${UACPP_PREFIX}/lib")
else()
    find_library(Uaclientcpp_LIBRARIES
        NAMES uaclientcpp
        HINTS "${UACPP_PREFIX}/lib")

    find_library(Uabasecpp_LIBRARIES
        NAMES uabasecpp
        HINTS "${UACPP_PREFIX}/lib")

    find_library(Uastack_LIBRARIES
        NAMES uastack
        HINTS "${UACPP_PREFIX}/lib")

    find_library(Uapkicpp_LIBRARIES
        NAMES uapkicpp
        HINTS "${UACPP_PREFIX}/lib")

    find_library(Xmlparsercpp_LIBRARIES
        NAMES xmlparsercpp
        HINTS "${UACPP_PREFIX}/lib")
endif()

if (NOT Uaclientcpp_INCLUDE_DIRS STREQUAL "Uaclientcpp_INCLUDE_DIRS-NOTFOUND"
    AND NOT Uabasecpp_INCLUDE_DIRS STREQUAL "Uabasecpp_INCLUDE_DIRS-NOTFOUND"
    AND NOT Uastack_INCLUDE_DIRS STREQUAL "Uastack_INCLUDE_DIRS-NOTFOUND"
    AND NOT Uapkicpp_INCLUDE_DIRS STREQUAL "Uapkicpp_INCLUDE_DIRS-NOTFOUND"
    AND NOT Xmlparsercpp_INCLUDE_DIRS STREQUAL "Xmlparsercpp_LIBRARIES-NOTFOUND"
    AND NOT Uaclientcpp_LIBRARIES STREQUAL "Uaclientcpp_LIBRARIES-NOTFOUND"
    AND NOT Uabasecpp_LIBRARIES STREQUAL "Uabasecpp_LIBRARIES-NOTFOUND"
    AND NOT Uastack_LIBRARIES STREQUAL "Uastack_LIBRARIES-NOTFOUND"
    AND NOT Uapkicpp_LIBRARIES STREQUAL "Uapkicpp_LIBRARIES-NOTFOUND"
    AND NOT Xmlparsercpp_LIBRARIES STREQUAL "Xmlparsercpp_LIBRARIES-NOTFOUND")
    set(Uacpp_FOUND ON)
endif()

if (Uacpp_FOUND)
    add_library(uaclientcpp UNKNOWN IMPORTED)
    set_target_properties(uaclientcpp PROPERTIES
        IMPORTED_LOCATION "${Uaclientcpp_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${Uaclientcpp_INCLUDE_DIRS}")

    add_library(uabasecpp UNKNOWN IMPORTED)
    set_target_properties(uabasecpp PROPERTIES
        IMPORTED_LOCATION "${Uabasecpp_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${Uabasecpp_INCLUDE_DIRS}")

    add_library(uastack UNKNOWN IMPORTED)
    set_target_properties(uastack PROPERTIES
        IMPORTED_LOCATION "${Uastack_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${Uastack_INCLUDE_DIRS}")

    add_library(uapkicpp UNKNOWN IMPORTED)
    set_target_properties(uapkicpp PROPERTIES
        IMPORTED_LOCATION "${Uapkicpp_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${Uapkicpp_INCLUDE_DIRS}")

    add_library(xmlparsercpp UNKNOWN IMPORTED)
    set_target_properties(xmlparsercpp PROPERTIES
        IMPORTED_LOCATION "${Xmlparsercpp_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${Xmlparsercpp_INCLUDE_DIRS}")

    add_library(uacpp INTERFACE IMPORTED)
    set_property(TARGET uacpp PROPERTY
        INTERFACE_LINK_LIBRARIES uaclientcpp uabasecpp uastack uapkicpp xmlparsercpp)

    if (WIN32)
        if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
            SET(UACPP_ARCHDIR "win64")
        else()
            SET(UACPP_ARCHDIR "win32")
        endif()

        find_library(Libxml2_LIBRARIES
            NAMES libxml2
            HINTS "${UACPP_PREFIX}/third-party/${UACPP_ARCHDIR}/vs2015/libxml2/out32dll")

        if (NOT Libxml2_LIBRARIES STREQUAL "Libxml2_LIBRARIES-NOTFOUND")
            add_library(libxml2 UNKNOWN IMPORTED)
            set_target_properties(libxml2 PROPERTIES
                IMPORTED_LOCATION "${Libxml2_LIBRARIES}")
        endif()
    endif()

    if (WIN32)
        set_property(TARGET uacpp APPEND PROPERTY
            INTERFACE_LINK_LIBRARIES Crypt32 Ole32 OleAut32 ws2_32 libeay32 libxml2)
    else()
        set_property(TARGET uacpp APPEND PROPERTY
            INTERFACE_LINK_LIBRARIES crypto ssl xml2)
    endif()
endif()

mark_as_advanced(Uaclientcpp_INCLUDE_DIRS Uaclientcpp_LIBRARIES
    Uabasecpp_INCLUDE_DIRS Uabasecpp_LIBRARIES
    Uastack_INCLUDE_DIRS Uastack_LIBRARIES
    Uapkicpp_INCLUDE_DIRS Uapkicpp_LIBRARIES
    Xmlparsercpp_INCLUDE_DIRS Xmlparsercpp_LIBRARIES)

include(FeatureSummary)
set_package_properties(Uacpp PROPERTIES
    URL "https://www.unified-automation.com/products/server-sdk/c-ua-server-sdk.html"
    DESCRIPTION "The Unified Automation C++ OPC UA SDK")
