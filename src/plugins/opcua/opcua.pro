TEMPLATE = subdirs

QT_FOR_CONFIG += opcua-private
qtConfig(freeopcua) {
    SUBDIRS += freeopcua
}

qtConfig(open62541) {
    SUBDIRS += open62541
}
