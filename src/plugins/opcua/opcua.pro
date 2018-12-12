TEMPLATE = subdirs

QT_FOR_CONFIG += opcua-private

!vxworks:!integrity {
    qtConfig(open62541)|qtConfig(system-open62541) {
        SUBDIRS += open62541
    }
}

qtConfig(uacpp) {
    SUBDIRS += uacpp
}
