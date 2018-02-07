TEMPLATE = subdirs
SUBDIRS += auto \
           manual

QT_FOR_CONFIG += opcua-private

qtConfig(open62541) {
    SUBDIRS += open62541-testserver
}

