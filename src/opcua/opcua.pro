TARGET = QtOpcUa
QT += core-private network
QT -= gui

include(core/core.pri)
include(client/client.pri)

MODULE_PLUGIN_TYPES = opcua
QMAKE_DOCS = $$PWD/doc/qtopcua.qdocconf

load(qt_module)

PUBLIC_HEADERS += qopcuaglobal.h

DEFINES += QT_NO_FOREACH

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

!qtConfig(ns0idnames): {
    DEFINES += QT_OPCUA_NO_NS0IDNAMES
}
