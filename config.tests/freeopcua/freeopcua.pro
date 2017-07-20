TEMPLATE = app

CONFIG += c++11 console

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += libopcuacore libopcuaclient libopcuaprotocol libopcuaserver
}

win32 {
    LIBS += opcuacore.lib opcuaclient.lib opcuaprotocol.lib opcuaserver.lib Wininet.lib
}

SOURCES += main.cpp
