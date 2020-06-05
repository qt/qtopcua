# Generated from auto.pro.

add_subdirectory(qopcuaclient)
add_subdirectory(connection)
add_subdirectory(security)
if(QT_FEATURE_uacpp OR (QT_FEATURE_open62541 AND TARGET Qt::QuickTest))
    add_subdirectory(declarative)
    add_subdirectory(clientSetupInCpp)
endif()
if(QT_FEATURE_gds AND QT_FEATURE_ssl AND NOT APPLE AND NOT WINRT)
    add_subdirectory(x509)
endif()
