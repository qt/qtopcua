TARGET = freeopcua_backend
QT += core core-private opcua opcua-private network concurrent

PLUGIN_TYPE = opcua
PLUGIN_CLASS_NAME = QFreeOpcUaPlugin
load(qt_plugin)

CONFIG += exceptions
DEFINES += QT_NO_FOREACH
QMAKE_USE_PRIVATE += freeopcua

# Input
HEADERS += \
           qfreeopcuanode.h \
           qfreeopcuasubscription.h \
           qfreeopcuaplugin.h \
           qfreeopcuavalueconverter.h \
           qfreeopcuaclient.h \
           qfreeopcuaworker.h

SOURCES += qfreeopcuaclient.cpp \
           qfreeopcuanode.cpp \
           qfreeopcuasubscription.cpp \
           qfreeopcuaplugin.cpp \
           qfreeopcuavalueconverter.cpp \
           qfreeopcuaworker.cpp

OTHER_FILES = freeopcua-metadata.json

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS
