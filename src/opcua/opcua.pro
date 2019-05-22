TARGET = QtOpcUa
QT += core-private network network-private
QT -= gui
QT_FOR_CONFIG += core-private

include(core/core.pri)
include(client/client.pri)
qtConfig(ssl):!darwin:!winrt: include(x509/x509.pri)

MODULE_PLUGIN_TYPES = opcua
QMAKE_DOCS = $$PWD/doc/qtopcua.qdocconf

load(qt_module)

PUBLIC_HEADERS += qopcuaglobal.h

DEFINES += QT_NO_FOREACH

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

!qtConfig(ns0idnames): {
    DEFINES += QT_OPCUA_NO_NS0IDNAMES
}
