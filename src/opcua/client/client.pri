# QQtOpcUa client module

PUBLIC_HEADERS += \
    client/qopcuaclient.h \
    client/qopcuasubscription.h \
    client/qopcuanode.h \
    client/qopcuatype.h \
    client/qopcuamonitoredevent.h \
    client/qopcuamonitoredvalue.h

SOURCES += \
    client/qopcuaclient.cpp \
    client/qopcuasubscription.cpp \
    client/qopcuanode.cpp \
    client/qopcuatype.cpp \
    client/qopcuamonitoredevent.cpp \
    client/qopcuamonitoredvalue.cpp \
    client/qopcuaclientimpl.cpp \
    client/qopcuanodeimpl.cpp \
    client/qopcuaclientprivate.cpp \
    client/qopcuamonitoredeventprivate.cpp \
    client/qopcuasubscriptionprivate.cpp \
    client/qopcuamonitoredvalueprivate.cpp \
    client/qopcuasubscriptionimpl.cpp

HEADERS += \
    client/qopcuaclient_p.h \
    client/qopcuaclientimpl_p.h \
    client/qopcuanode_p.h \
    client/qopcuanodeimpl_p.h \
    client/qopcuamonitoredevent_p.h \
    client/qopcuamonitoredvalue_p.h \
    client/qopcuasubscription_p.h \
    client/qopcuasubscriptionimpl_p.h
