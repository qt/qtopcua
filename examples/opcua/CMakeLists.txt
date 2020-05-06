# Generated from opcua.pro.

if(TARGET Qt::Widgets)
    add_subdirectory(opcuaviewer)
endif()
if(QT_FEATURE_gds AND QT_FEATURE_ssl AND NOT APPLE AND NOT WINRT)
    add_subdirectory(x509)
endif()
if(QT_FEATURE_open62541 AND TARGET Qt::Quick)
    add_subdirectory(waterpump)
endif()
