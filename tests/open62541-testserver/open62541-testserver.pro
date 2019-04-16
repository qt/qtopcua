TEMPLATE = app
TARGET = open62541-testserver

INCLUDEPATH += \
               $$PWD/../../src/plugins/opcua/open62541

DEPENDPATH += INCLUDEPATH

CONFIG += c++11 console

QT += opcua-private

qtConfig(open62541):!qtConfig(system-open62541) {
    qtConfig(mbedtls):{
        QMAKE_USE_PRIVATE += mbedtls
        DEFINES += UA_ENABLE_ENCRYPTION
    }
    include($$PWD/../../src/3rdparty/open62541.pri)
} else {
    QMAKE_USE_PRIVATE += open62541
}

win32: DESTDIR = ./

# Workaround for QTBUG-75020
QMAKE_CFLAGS_RELEASE -= -O2
QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO -= -O2

# This file  can only be compiled in case of TLS support
qtConfig(mbedtls) {
    # Use custom compiler from src/3rdparty/open62541.pri to hide warning caused by
    # including open62541.h
    OPEN62541_SOURCES += security_addon.cpp
}

SOURCES += \
           main.cpp \
           testserver.cpp \
           $$PWD/../../src/plugins/opcua/open62541/qopen62541utils.cpp \
           $$PWD/../../src/plugins/opcua/open62541/qopen62541valueconverter.cpp


HEADERS += \
           testserver.h \
           security_addon.h \

RESOURCES += certs.qrc

