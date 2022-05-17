# Generated from auto.pro.

if(NOT ANDROID)
    add_subdirectory(qopcuaclient)
    add_subdirectory(connection)
    add_subdirectory(security)
    if(TARGET Qt::QuickTest AND (QT_FEATURE_open62541 OR QT_FEATURE_uacpp))
        add_subdirectory(declarative)
        add_subdirectory(clientSetupInCpp)
    endif()
    if(QT_FEATURE_gds AND QT_FEATURE_ssl AND NOT APPLE AND NOT WINRT)
        add_subdirectory(x509)
    endif()
endif()
