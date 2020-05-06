# Generated from imports.pro.

add_subdirectory(doc)
if(TARGET Qt::OpcUa)
    add_subdirectory(opcua)
endif()
