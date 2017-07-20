TARGET = freeopcua_backend
QT += core core-private opcua opcua-private network

PLUGIN_TYPE = opcua
PLUGIN_CLASS_NAME = QFreeOpcUaPlugin
load(qt_plugin)

CONFIG += exceptions link_pkgconfig
unix: PKGCONFIG += libopcuaprotocol libopcuacore libopcuaclient
win32: LIBS += opcuaprotocol.lib opcuacore.lib opcuaclient.lib Wininet.lib

# Input
HEADERS += \
           qfreeopcuanode.h \
           qfreeopcuavaluesubscription.h \
           qfreeopcuaplugin.h \
           qfreeopcuavalueconverter.h \
           qfreeopcuaclient.h

SOURCES += qfreeopcuaclient.cpp \
           qfreeopcuanode.cpp \
           qfreeopcuavaluesubscription.cpp \
           qfreeopcuaplugin.cpp \
           qfreeopcuavalueconverter.cpp

OTHER_FILES = freeopcua-metadata.json

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS
