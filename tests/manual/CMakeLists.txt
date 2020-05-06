# Generated from manual.pro.

add_subdirectory(eventsubscription)
if(QT_FEATURE_ssl AND NOT APPLE AND NOT WINRT)
    add_subdirectory(gds)
endif()
