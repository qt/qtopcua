# Generated from connection.pro.

#####################################################################
## tst_connection Test:
#####################################################################

qt_add_test(tst_connection
    SOURCES
        ../../common/backend_environment.h
        tst_connection.cpp
    INCLUDE_DIRECTORIES
        ../../common
    PUBLIC_LIBRARIES
        Qt::OpcUa
)
