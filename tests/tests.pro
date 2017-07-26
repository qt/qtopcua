TEMPLATE = subdirs
SUBDIRS += auto

QT_FOR_CONFIG += opcua-private
qtConfig(freeopcua) {
    SUBDIRS += freeopcua-testserver
}
