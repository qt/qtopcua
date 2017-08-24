TEMPLATE = app
TARGET = open62541-testserver

INCLUDEPATH += \
               $$PWD/../../src/plugins/opcua/open62541

DEPENDPATH += INCLUDEPATH

CONFIG += c++11 console

QMAKE_USE_PRIVATE += open62541
win32: DESTDIR = ./

SOURCES += \
           main.cpp \
           testserver.cpp

HEADERS += \
           testserver.h
