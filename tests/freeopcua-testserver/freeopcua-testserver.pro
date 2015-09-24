TEMPLATE = app
TARGET = freeopcua-testserver

INCLUDEPATH += .

CONFIG += c++11
CONFIG += exceptions

CONFIG += exceptions link_pkgconfig
PKGCONFIG += libopcuaprotocol libopcuacore libopcuaserver

SOURCES += main.cpp \
           accontrol.cpp

HEADERS += accontrol.h
