TARGET = uacpp_backend
QT += core core-private opcua opcua-private network
QT -= gui
QMAKE_USE += uacpp

win32: DEFINES += _UA_STACK_USE_DLL

HEADERS += \
    quacppbackend.h \
    quacppclient.h \
    quacppnode.h \
    quacppplugin.h \
    quacppsubscription.h \
    quacppvalueconverter.h \
    quacpputils.h

SOURCES += \
    quacppbackend.cpp \
    quacppclient.cpp \
    quacppnode.cpp \
    quacppplugin.cpp \
    quacppsubscription.cpp \
    quacppvalueconverter.cpp \
    quacpputils.cpp

OTHER_FILES = uacpp-metadata.json

PLUGIN_TYPE = opcua
PLUGIN_CLASS_NAME = QUACppPlugin
load(qt_plugin)
