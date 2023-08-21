# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

qt_internal_add_qml_module(DeclarativeOpcua
    URI "QtOpcUa"
    VERSION "${PROJECT_VERSION}"
    PAST_MAJOR_VERSIONS 1 5
    DEPENDENCIES
        QtQuick
    SOURCES
        opcuaattributecache.cpp opcuaattributecache_p.h
        opcuaattributeoperand.cpp opcuaattributeoperand_p.h
        opcuaattributevalue.cpp opcuaattributevalue_p.h
        opcuaconnection.cpp opcuaconnection_p.h
        opcuadatachangefilter.cpp opcuadatachangefilter_p.h
        opcuaelementoperand.cpp opcuaelementoperand_p.h
        opcuaendpointdiscovery.cpp opcuaendpointdiscovery_p.h
        opcuaeventfilter.cpp opcuaeventfilter_p.h
        opcuafilterelement.cpp opcuafilterelement_p.h
        opcuaforeign.cpp opcuaforeign_p.h
        opcualiteraloperand.cpp opcualiteraloperand_p.h
        opcuamethodargument.cpp opcuamethodargument_p.h
        opcuamethodnode.cpp opcuamethodnode_p.h
        opcuanode.cpp opcuanode_p.h
        opcuanodeid.cpp opcuanodeid_p.h
        opcuanodeidtype.cpp opcuanodeidtype_p.h
        opcuaoperandbase.cpp opcuaoperandbase_p.h
        opcuapathresolver.cpp opcuapathresolver_p.h
        opcuareaditem.cpp opcuareaditem_p.h
        opcuareadresult.cpp opcuareadresult_p.h
        opcuarelativenodeid.cpp opcuarelativenodeid_p.h
        opcuarelativenodepath.cpp opcuarelativenodepath_p.h
        opcuaserverdiscovery.cpp opcuaserverdiscovery_p.h
        opcuasimpleattributeoperand.cpp opcuasimpleattributeoperand_p.h
        opcuastatus.cpp opcuastatus_p.h
        opcuavaluenode.cpp opcuavaluenode_p.h
        opcuawriteitem.cpp opcuawriteitem_p.h
        opcuawriteresult.cpp opcuawriteresult_p.h
        universalnode.cpp universalnode_p.h
    LIBRARIES
        Qt::CorePrivate
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Gui
        Qt::OpcUa
        Qt::Quick
)

# The file contains Q_OBJECT in a comment
set_property(SOURCE opcuaconnection.cpp PROPERTY SKIP_AUTOMOC ON)
