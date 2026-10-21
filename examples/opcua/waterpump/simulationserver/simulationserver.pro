TEMPLATE = app
CONFIG += c++11 console
QT += opcua
QT -= gui

isEmpty(QTOPCUA_OPEN62541_SOURCE_DIR) {
  # Defaults for builds from the source tree or next to an installed Qt
  QTOPCUA_OPEN62541_SOURCE_DIR=$$PWD/../../../../src/3rdparty/open62541
  !exists($$QTOPCUA_OPEN62541_SOURCE_DIR/open62541.c): QTOPCUA_OPEN62541_SOURCE_DIR=$$PWD/../../../../../$$QT_VERSION/Src/qtopcua/src/3rdparty/open62541
}

!exists($$QTOPCUA_OPEN62541_SOURCE_DIR/open62541.c) {
  error("Unable to build the server, open62541.c was not found. Set QTOPCUA_OPEN62541_SOURCE_DIR to the directory containing open62541.c (src/3rdparty/open62541 in the Qt OPC UA sources).")
}

INCLUDEPATH += $$QTOPCUA_OPEN62541_SOURCE_DIR
DEPENDPATH += $$QTOPCUA_OPEN62541_SOURCE_DIR
HEADERS += $$QTOPCUA_OPEN62541_SOURCE_DIR/open62541.h
SOURCES += $$QTOPCUA_OPEN62541_SOURCE_DIR/open62541.c

win32-msvc: LIBS += ws2_32.lib iphlpapi.lib
win32-g++: LIBS += -lws2_32 -liphlpapi

macos|ios: DEFINES += _DARWIN_C_SOURCE

!win32-msvc: QMAKE_CFLAGS += -Wno-unused-parameter

SOURCES += main.cpp \
    simulationserver.cpp

HEADERS += \
    simulationserver.h

#install
target.path = $$[QT_INSTALL_EXAMPLES]/opcua/waterpump/simulationserver
INSTALLS += target
