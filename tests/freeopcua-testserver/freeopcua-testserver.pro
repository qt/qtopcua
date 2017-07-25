TEMPLATE = app
TARGET = freeopcua-testserver

INCLUDEPATH += .

CONFIG += c++11 console
CONFIG += exceptions

CONFIG += exceptions link_pkgconfig
unix: PKGCONFIG += libopcuaprotocol libopcuacore libopcuaserver
win32 {
    LIBS += opcuacore.lib opcuaprotocol.lib opcuaserver.lib Wininet.lib
    QMAKE_LFLAGS += -STACK:3000000
    DESTDIR = ./
}

SOURCES += main.cpp \
           accontrol.cpp

HEADERS += accontrol.h
