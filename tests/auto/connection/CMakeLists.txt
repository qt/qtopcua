# Generated from connection.pro.

#####################################################################
## tst_connection Test:
#####################################################################

qt_internal_add_test(tst_connection
    SOURCES
        ../../common/backend_environment.h
        tst_connection.cpp
    INCLUDE_DIRECTORIES
        ../../common
    PUBLIC_LIBRARIES
        Qt::OpcUa
)

# special case begin
if (WIN32)
    target_compile_definitions(tst_connection PRIVATE TESTS_CMAKE_SPECIFIC_PATH)
endif()
# special case end
