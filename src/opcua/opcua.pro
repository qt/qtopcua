TARGET = QtOpcUa
QT += core-private

include(core/core.pri)
include(client/client.pri)

MODULE_PLUGIN_TYPES = opcua
QMAKE_DOCS = $$PWD/doc/qtopcua.qdocconf

load(qt_module)

PUBLIC_HEADERS += qopcuaglobal.h

DEFINES += QT_NO_FOREACH

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS
