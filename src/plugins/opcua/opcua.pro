TEMPLATE = subdirs

QT_FOR_CONFIG += opcua-private
qtConfig(freeopcua) {
    SUBDIRS += freeopcua
}
