#.rst:
# FindMbedtls
# ---------
#
# Try to locate the mbedTLS library.
# If found, this will define the following variables:
#
# ``Mbedtls_FOUND``
#     True if the mbedtls library is available
# ``Mbedtls_INCLUDE_DIRS``
#     The mbedTLS include directories
# ``Mbedx509_LIBRARIES``
#     The mbedx509 library for linking
# ``Mbedcrypto_LIBRARIES``
#     The mbedcrypto library for linking
#
# If ``Mbedtls_FOUND`` is TRUE, it will also define the following
# imported targets:
#
# ``mbedx509``
#     The mbedx509 library
# ``mbedcrypto``
#     The mbedcrypto library
# ``mbedtls``
#     An interface library combining mbedx509 and mbedcrypto


find_path(Mbedtls_INCLUDE_DIRS
    NAMES mbedtls/x509_crt.h
    HINTS ${MBEDTLS_INCDIR})

find_library(Mbedx509_LIBRARIES
    NAMES mbedx509
    HINTS ${MBEDTLS_LIBDIR})

find_library(Mbedcrypto_LIBRARIES
    NAMES mbedcrypto
    HINTS ${MBEDTLS_LIBDIR})

if (NOT Mbedtls_INCLUDE_DIRS STREQUAL "Mbedtls_INCLUDE_DIRS-NOTFOUND"
    AND NOT Mbedx509_LIBRARIES STREQUAL "Mbedx09_LIBRARIES-NOTFOUND"
    AND NOT Mbedcrypto_LIBRARIES STREQUAL "Mbedcrypto_LIBRARIES-NOTFOUND")
    set(Mbedtls_FOUND ON)
endif()

if (Mbedtls_FOUND)
    add_library(mbedx509 UNKNOWN IMPORTED)
    set_target_properties(mbedx509 PROPERTIES
        IMPORTED_LOCATION "${Mbedx509_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${Mbedtls_INCLUDE_DIRS}")

    add_library(mbedcrypto UNKNOWN IMPORTED)
    set_target_properties(mbedcrypto PROPERTIES
        IMPORTED_LOCATION "${Mbedcrypto_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${Mbedtls_INCLUDE_DIRS}")

    add_library(mbedtls INTERFACE IMPORTED)
    set_property(TARGET mbedtls PROPERTY
        INTERFACE_LINK_LIBRARIES mbedx509 mbedcrypto)
endif()

mark_as_advanced(Mbedtls_INCLUDE_DIRS Mbedx509_LIBRARIES Mbedcrypto_LIBRARIES)

include(FeatureSummary)
set_package_properties(Mbedtls PROPERTIES
    URL "https://tls.mbed.org/"
    DESCRIPTION "The mbedTLS library")
