# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

#####################################################################
## OpcUa Module:
#####################################################################

qt_internal_add_module(OpcUa
    PLUGIN_TYPES opcua
    SOURCES
        client/qopcuaaddnodeitem.cpp client/qopcuaaddnodeitem.h
        client/qopcuaaddreferenceitem.cpp client/qopcuaaddreferenceitem.h
        client/qopcuaapplicationdescription.cpp client/qopcuaapplicationdescription.h
        client/qopcuaapplicationidentity.cpp client/qopcuaapplicationidentity.h
        client/qopcuaapplicationrecorddatatype.cpp client/qopcuaapplicationrecorddatatype.h
        client/qopcuaargument.cpp client/qopcuaargument.h
        client/qopcuaattributeoperand.cpp client/qopcuaattributeoperand.h
        client/qopcuaauthenticationinformation.cpp client/qopcuaauthenticationinformation.h
        client/qopcuaaxisinformation.cpp client/qopcuaaxisinformation.h
        client/qopcuabackend.cpp client/qopcuabackend_p.h
        client/qopcuabinarydataencoding.cpp client/qopcuabinarydataencoding.h
        client/qopcuabrowsepathtarget.cpp client/qopcuabrowsepathtarget.h
        client/qopcuabrowserequest.cpp client/qopcuabrowserequest.h
        client/qopcuaclient.cpp client/qopcuaclient.h client/qopcuaclient_p.h
        client/qopcuaclientimpl.cpp client/qopcuaclientimpl_p.h
        client/qopcuaclientprivate.cpp
        client/qopcuacomplexnumber.cpp client/qopcuacomplexnumber.h
        client/qopcuaconnectionsettings.cpp client/qopcuaconnectionsettings.h
        client/qopcuacontentfilterelement.cpp client/qopcuacontentfilterelement.h
        client/qopcuacontentfilterelementresult.cpp client/qopcuacontentfilterelementresult.h
        client/qopcuadatavalue.cpp client/qopcuadatavalue.h
        client/qopcuadeletereferenceitem.cpp client/qopcuadeletereferenceitem.h
        client/qopcuadoublecomplexnumber.cpp client/qopcuadoublecomplexnumber.h
        client/qopcuaelementoperand.cpp client/qopcuaelementoperand.h
        client/qopcuaendpointdescription.cpp client/qopcuaendpointdescription.h
        client/qopcuaerrorstate.cpp client/qopcuaerrorstate.h
        client/qopcuaeuinformation.cpp client/qopcuaeuinformation.h
        client/qopcuaeventfilterresult.cpp client/qopcuaeventfilterresult.h
        client/qopcuaexpandednodeid.cpp client/qopcuaexpandednodeid.h
        client/qopcuaextensionobject.cpp client/qopcuaextensionobject.h
        client/qopcuahistorydata.cpp client/qopcuahistorydata.h
        client/qopcuahistoryreadrawrequest.cpp client/qopcuahistoryreadrawrequest.h
        client/qopcuahistoryreadresponse.cpp client/qopcuahistoryreadresponse.h
        client/qopcuahistoryreadresponseimpl.cpp client/qopcuahistoryreadresponseimpl_p.h
        client/qopcuahistoryreadresponse_p.h
        client/qopcualiteraloperand.cpp client/qopcualiteraloperand.h
        client/qopcualocalizedtext.cpp client/qopcualocalizedtext.h
        client/qopcuamonitoringparameters.cpp client/qopcuamonitoringparameters.h client/qopcuamonitoringparameters_p.h
        client/qopcuamultidimensionalarray.cpp client/qopcuamultidimensionalarray.h
        client/qopcuanode.cpp client/qopcuanode.h client/qopcuanode_p.h
        client/qopcuanodecreationattributes.cpp client/qopcuanodecreationattributes.h client/qopcuanodecreationattributes_p.h
        client/qopcuanodeids.cpp client/qopcuanodeids.h
        client/qopcuanodeimpl.cpp client/qopcuanodeimpl_p.h
        client/qopcuapkiconfiguration.cpp client/qopcuapkiconfiguration.h
        client/qopcuaqualifiedname.cpp client/qopcuaqualifiedname.h
        client/qopcuarange.cpp client/qopcuarange.h
        client/qopcuareaditem.cpp client/qopcuareaditem.h
        client/qopcuareadresult.cpp client/qopcuareadresult.h
        client/qopcuareferencedescription.cpp client/qopcuareferencedescription.h
        client/qopcuarelativepathelement.cpp client/qopcuarelativepathelement.h
        client/qopcuasimpleattributeoperand.cpp client/qopcuasimpleattributeoperand.h
        client/qopcuatype.cpp client/qopcuatype.h
        client/qopcuausertokenpolicy.cpp client/qopcuausertokenpolicy.h
        client/qopcuawriteitem.cpp client/qopcuawriteitem.h
        client/qopcuawriteresult.cpp client/qopcuawriteresult.h
        client/qopcuaxvalue.cpp client/qopcuaxvalue.h
        core/qopcuaplugin.cpp core/qopcuaplugin.h
        core/qopcuaprovider.cpp core/qopcuaprovider.h
        qopcuaglobal.h

    DEFINES
        QT_NO_FOREACH
    LIBRARIES
        Qt::CorePrivate
        Qt::NetworkPrivate
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::Network
    PRIVATE_MODULE_INTERFACE
        Qt::CorePrivate
        Qt::NetworkPrivate
    GENERATE_CPP_EXPORTS
)

## Scopes:
#####################################################################

qt_internal_extend_target(OpcUa
    CONDITION
        QT_FEATURE_gds AND QT_FEATURE_openssl AND NOT APPLE AND NOT WINRT
    SOURCES
        client/qopcuagdsclient.cpp client/qopcuagdsclient_p.h
        x509/openssl_symbols.cpp
        x509/qopcuakeypair.cpp x509/qopcuakeypair.h
        x509/qopcuakeypair_openssl.cpp
        x509/qopcuax509certificatesigningrequest.cpp x509/qopcuax509certificatesigningrequest.h
        x509/qopcuax509certificatesigningrequest_openssl.cpp
        x509/qopcuax509distinguishedname.cpp x509/qopcuax509distinguishedname.h
        x509/qopcuax509extension.cpp x509/qopcuax509extension.h
        x509/qopcuax509extensionbasicconstraints.cpp x509/qopcuax509extensionbasicconstraints.h
        x509/qopcuax509extensionextendedkeyusage.cpp x509/qopcuax509extensionextendedkeyusage.h
        x509/qopcuax509extensionkeyusage.cpp x509/qopcuax509extensionkeyusage.h
        x509/qopcuax509extensionsubjectalternativename.cpp
        x509/qopcuax509extensionsubjectalternativename.h
        x509/qopcuax509utils.cpp x509/qopcuax509utils_p.h
)

if (QT_FEATURE_gds AND QT_FEATURE_openssl)
    qt_internal_extend_target(OpcUa
        LIBRARIES WrapOpenSSL::WrapOpenSSL)
endif()

if(MSVC)
    target_compile_options(OpcUa PRIVATE /constexpr:steps3000000)
endif()

qt_internal_extend_target(OpcUa CONDITION NOT QT_FEATURE_ns0idnames
    DEFINES
        QT_OPCUA_NO_NS0IDNAMES
)
qt_internal_add_docs(OpcUa
    doc/qtopcua.qdocconf
)
