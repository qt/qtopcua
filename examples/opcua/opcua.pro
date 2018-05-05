TEMPLATE = subdirs
SUBDIRS += \
            opcuaviewer \

QT_FOR_CONFIG += opcua-private

qtConfig(open62541) {
    qtHaveModule(quick): SUBDIRS += waterpump
}
