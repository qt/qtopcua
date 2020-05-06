QT += quick opcua

QML_IMPORT_VERSION = 1.0

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
    opcuaserverdiscovery.cpp \
    opcuastatus.cpp \
    opcuawriteitem.cpp \
    opcuawriteresult.cpp \
    opcuadatachangefilter.cpp \
    opcuaelementoperand.cpp \
    opcualiteraloperand.cpp \
    opcuasimpleattributeoperand.cpp \
    opcuaattributeoperand.cpp \
    opcuafilterelement.cpp \
    opcuaeventfilter.cpp \
    opcuaoperandbase.cpp \

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
    opcuaserverdiscovery.h \
    opcuastatus.h \
    opcuawriteitem.h \
    opcuawriteresult.h \
    opcuadatachangefilter.h \
    opcuaelementoperand.h \
    opcualiteraloperand.h \
    opcuasimpleattributeoperand.h \
    opcuaattributeoperand.h \
    opcuafilterelement.h \
    opcuaeventfilter.h \
    opcuaoperandbase.h \

load(qml_plugin)

OTHER_FILES += \
    plugin.json \
    qmldir \
