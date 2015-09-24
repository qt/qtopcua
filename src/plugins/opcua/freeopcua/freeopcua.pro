TARGET = freeopcua_backend
QT = core opcua

PLUGIN_TYPE = opcua
PLUGIN_CLASS_NAME = QFreeOpcUaPlugin
load(qt_plugin)

CONFIG += exceptions link_pkgconfig
PKGCONFIG += libopcuaprotocol libopcuacore libopcuaclient

# Input
HEADERS += qfreeopcuaclient.h \
           qfreeopcuanode.h \
           qfreeopcuasubscription.h \
           qfreeopcuaplugin.h \
           qfreeopcuavalueconverter.h \
           qfreeopcuamonitoreditem.h \

SOURCES += qfreeopcuaclient.cpp \
           qfreeopcuanode.cpp \
           qfreeopcuasubscription.cpp \
           qfreeopcuaplugin.cpp \
           qfreeopcuavalueconverter.cpp \
           qfreeopcuamonitoreditem.cpp \

OTHER_FILES = freeopcua-metadata.json
