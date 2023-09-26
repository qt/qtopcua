TEMPLATE = subdirs

QT_FOR_CONFIG += opcua-private

qtConfig(internalgenerator): {
    SUBDIRS += internalgenerator
}
