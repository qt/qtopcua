TEMPLATE = subdirs
QT_FOR_CONFIG += opcua-private
SUBDIRS += doc
qtHaveModule(opcua): SUBDIRS += opcua
