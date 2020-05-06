# Generated from src.pro.

add_subdirectory(opcua)
add_subdirectory(plugins)
if(TARGET Qt::Quick)
    add_subdirectory(imports)
endif()
