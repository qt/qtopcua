TEMPLATE = app

CONFIG += c++11

unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += libopcuacore libopcuaclient libopcuaprotocol libopcuaserver
}
SOURCES += main.cpp
