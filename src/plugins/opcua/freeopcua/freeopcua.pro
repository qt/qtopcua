TARGET = freeopcua_backend
QT += core core-private opcua opcua-private network concurrent

PLUGIN_TYPE = opcua
PLUGIN_CLASS_NAME = QFreeOpcUaPlugin
load(qt_plugin)

CONFIG += exceptions link_pkgconfig
DEFINES += QT_NO_FOREACH
unix: PKGCONFIG += libopcuaprotocol libopcuacore libopcuaclient
win32: LIBS += opcuaprotocol.lib opcuacore.lib opcuaclient.lib Wininet.lib

# Input
HEADERS += \
           qfreeopcuanode.h \
           qfreeopcuasubscription.h \
           qfreeopcuaplugin.h \
           qfreeopcuavalueconverter.h \
           qfreeopcuaclient.h

SOURCES += qfreeopcuaclient.cpp \
           qfreeopcuanode.cpp \
           qfreeopcuasubscription.cpp \
           qfreeopcuaplugin.cpp \
           qfreeopcuavalueconverter.cpp

OTHER_FILES = freeopcua-metadata.json

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS
