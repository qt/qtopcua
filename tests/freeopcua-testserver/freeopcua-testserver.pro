TEMPLATE = app
TARGET = freeopcua-testserver

INCLUDEPATH += .

CONFIG += c++11 console
CONFIG += exceptions

QMAKE_USE_PRIVATE += freeopcua
win32 {
    QMAKE_LFLAGS += -STACK:3000000
    DESTDIR = ./
}

SOURCES += main.cpp \
           accontrol.cpp

HEADERS += accontrol.h
