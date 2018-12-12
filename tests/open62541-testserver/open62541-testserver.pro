TEMPLATE = app
TARGET = open62541-testserver

INCLUDEPATH += \
               $$PWD/../../src/plugins/opcua/open62541

DEPENDPATH += INCLUDEPATH

CONFIG += c++11 console

QT += opcua-private

qtConfig(open62541):!qtConfig(system-open62541) {
    include($$PWD/../../src/3rdparty/open62541.pri)
} else {
    QMAKE_USE_PRIVATE += open62541
}

win32: DESTDIR = ./

SOURCES += \
           main.cpp \
           testserver.cpp \
           $$PWD/../../src/plugins/opcua/open62541/qopen62541utils.cpp \
           $$PWD/../../src/plugins/opcua/open62541/qopen62541valueconverter.cpp


HEADERS += \
           testserver.h
