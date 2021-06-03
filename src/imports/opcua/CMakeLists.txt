qt_internal_add_qml_module(declarative_opcua
    URI "QtOpcUa"
    VERSION "${PROJECT_VERSION}"
    PAST_MAJOR_VERSIONS 1 5
    CLASS_NAME "OpcUaPlugin"
    PLUGIN_TARGET declarative_opcua
    NO_PLUGIN_OPTIONAL
    NO_GENERATE_PLUGIN_SOURCE
    NO_GENERATE_QMLTYPES
    INSTALL_SOURCE_QMLTYPES "plugins.qmltypes"
    DEPENDENCIES
        QtQuick
    SOURCES
        opcua_plugin.cpp opcua_plugin.h
        opcuaattributecache.cpp opcuaattributecache.h
        opcuaattributeoperand.cpp opcuaattributeoperand.h
        opcuaattributevalue.cpp opcuaattributevalue.h
        opcuaconnection.cpp opcuaconnection.h
        opcuadatachangefilter.cpp opcuadatachangefilter.h
        opcuaelementoperand.cpp opcuaelementoperand.h
        opcuaendpointdiscovery.cpp opcuaendpointdiscovery.h
        opcuaeventfilter.cpp opcuaeventfilter.h
        opcuafilterelement.cpp opcuafilterelement.h
        opcualiteraloperand.cpp opcualiteraloperand.h
        opcuamethodargument.cpp opcuamethodargument.h
        opcuamethodnode.cpp opcuamethodnode.h
        opcuanode.cpp opcuanode.h
        opcuanodeid.cpp opcuanodeid.h
        opcuanodeidtype.cpp opcuanodeidtype.h
        opcuaoperandbase.cpp opcuaoperandbase.h
        opcuapathresolver.cpp opcuapathresolver.h
        opcuareaditem.cpp opcuareaditem.h
        opcuareadresult.cpp opcuareadresult.h
        opcuarelativenodeid.cpp opcuarelativenodeid.h
        opcuarelativenodepath.cpp opcuarelativenodepath.h
        opcuaserverdiscovery.cpp opcuaserverdiscovery.h
        opcuasimpleattributeoperand.cpp opcuasimpleattributeoperand.h
        opcuastatus.cpp opcuastatus.h
        opcuavaluenode.cpp opcuavaluenode.h
        opcuawriteitem.cpp opcuawriteitem.h
        opcuawriteresult.cpp opcuawriteresult.h
        universalnode.cpp universalnode.h
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Gui
        Qt::OpcUa
        Qt::Quick
)

# The file contains Q_OBJECT in a comment
set_property(SOURCE opcuaconnection.cpp PROPERTY SKIP_AUTOMOC ON)
