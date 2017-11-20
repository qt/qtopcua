TEMPLATE = app
TARGET = open62541-testserver

INCLUDEPATH += \
               $$PWD/../../src/plugins/opcua/open62541

DEPENDPATH += INCLUDEPATH

CONFIG += c++11 console

QT += opcua-private

QMAKE_USE_PRIVATE += open62541
win32: DESTDIR = ./

SOURCES += \
           main.cpp \
           testserver.cpp \
           $$PWD/../../src/plugins/opcua/open62541/qopen62541utils.cpp


HEADERS += \
           testserver.h
