QT += quick opcua

SOURCES += \
    opcua_plugin.cpp \
    opcuaconnection.cpp \
    opcuaendpointdiscovery.cpp \
    opcuanode.cpp \
    opcuamethodnode.cpp \
    opcuavaluenode.cpp \
    opcuanodeid.cpp \
    opcuarelativenodepath.cpp \
    opcuarelativenodeid.cpp \
    opcuanodeidtype.cpp \
    universalnode.cpp \
    opcuapathresolver.cpp \
    opcuaattributevalue.cpp \
    opcuaattributecache.cpp \
    opcuamethodargument.cpp \
    opcuareaditem.cpp \
    opcuareadresult.cpp \
    opcuastatus.cpp \

HEADERS += \
    opcua_plugin.h \
    opcuaconnection.h \
    opcuaendpointdiscovery.h \
    opcuanode.h \
    opcuamethodnode.h \
    opcuavaluenode.h \
    opcuanodeid.h \
    opcuarelativenodepath.h \
    opcuarelativenodeid.h \
    opcuanodeidtype.h \
    universalnode.h \
    opcuapathresolver.h \
    opcuaattributecache.h \
    opcuaattributevalue.h \
    opcuamethodargument.h \
    opcuareaditem.h \
    opcuareadresult.h \
    opcuastatus.h \

load(qml_plugin)

OTHER_FILES += \
    plugin.json \
    qmldir \
