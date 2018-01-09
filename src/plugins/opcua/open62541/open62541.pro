TARGET = open62541_backend
QT += core core-private opcua opcua-private network

PLUGIN_TYPE = opcua
PLUGIN_CLASS_NAME = QOpen62541Plugin
load(qt_plugin)

QMAKE_USE_PRIVATE += open62541
win32-msvc: LIBS += open62541.lib ws2_32.lib

HEADERS += \
    qopen62541backend.h \
    qopen62541client.h \
    qopen62541node.h \
    qopen62541plugin.h \
    qopen62541subscription.h \
    qopen62541valueconverter.h \
    qopen62541.h \
    qopen62541utils.h

SOURCES += \
    qopen62541backend.cpp \
    qopen62541client.cpp \
    qopen62541node.cpp \
    qopen62541plugin.cpp \
    qopen62541subscription.cpp \
    qopen62541valueconverter.cpp \
    qopen62541utils.cpp

OTHER_FILES = open62541-metadata.json
