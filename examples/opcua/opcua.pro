TEMPLATE = subdirs
qtHaveModule(widgets): SUBDIRS += \
            opcuaviewer \

QT_FOR_CONFIG += opcua-private core-private

qtConfig(gds) {
    qtConfig(ssl):!darwin:!winrt: SUBDIRS += x509
}

qtConfig(open62541) {
    qtHaveModule(quick): SUBDIRS += waterpump
}
