TARGET = uacpp_backend
QT += core core-private opcua opcua-private network

win32 {
    CONFIG(debug, debug|release): LIBS += uaclientd.lib uabased.lib coremoduled.lib uastackd.lib uapkid.lib xmlparserd.lib
    else: LIBS += uaclient.lib uabase.lib coremodule.lib uastack.lib uapki.lib xmlparser.lib

    LIBS += Crypt32.lib Ole32.lib OleAut32.lib ws2_32.lib

    # The UA SDK bundles hardcoded builds of libxml and openssl. Preferably we should get rid of
    # this at some point.
    LIBS += libeay32.lib libxml2.lib

    DEFINES += _UA_STACK_USE_DLL
}
unix {
    LIBS += -luaclient -luamodule -luamodels -lcoremodule -luabase -luastack -lxmlparser -luapki -lcrypto -lssl -lxml2
}

HEADERS += \
    quacppbackend.h \
    quacppclient.h \
    quacppnode.h \
    quacppplugin.h \
    quacppsubscription.h \
    quacppvalueconverter.h \
    quacpputils.h

SOURCES += \
    quacppbackend.cpp \
    quacppclient.cpp \
    quacppnode.cpp \
    quacppplugin.cpp \
    quacppsubscription.cpp \
    quacppvalueconverter.cpp \
    quacpputils.cpp

OTHER_FILES = uacpp-metadata.json

PLUGIN_TYPE = opcua
PLUGIN_CLASS_NAME = QUACppPlugin
load(qt_plugin)
