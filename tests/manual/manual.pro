TEMPLATE = subdirs

QT_FOR_CONFIG += opcua-private core-private
qtConfig(ssl):!darwin:!winrt: SUBDIRS += gds
