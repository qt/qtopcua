# QQtOpcUa client module

PUBLIC_HEADERS += \
    client/qopcuaclient.h \
    client/qopcuanode.h \
    client/qopcuatype.h

SOURCES += \
    client/qopcuaclient.cpp \
    client/qopcuanode.cpp \
    client/qopcuatype.cpp \
    client/qopcuaclientimpl.cpp \
    client/qopcuanodeimpl.cpp \
    client/qopcuaclientprivate.cpp \
    client/qopcuabackend.cpp \
    client/qopcuamonitoringparameters.cpp \
    client/qopcuabinarydataencoding.cpp \
    client/qopcuabrowsing.cpp \
    client/qopcuareaditem.cpp \
    client/qopcuareadresult.cpp

HEADERS += \
    client/qopcuaclient_p.h \
    client/qopcuaclientimpl_p.h \
    client/qopcuanode_p.h \
    client/qopcuanodeimpl_p.h \
    client/qopcuabackend_p.h \
    client/qopcuamonitoringparameters.h \
    client/qopcuamonitoringparameters_p.h \
    client/qopcuabinarydataencoding.h \
    client/qopcuabrowsing.h \
    client/qopcuareaditem.h \
    client/qopcuareadresult.h
