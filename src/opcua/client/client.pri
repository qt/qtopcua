# QQtOpcUa client module

PUBLIC_HEADERS += \
    client/qopcuaclient.h \
    client/qopcuanode.h \
    client/qopcuaapplicationidentity.h \
    client/qopcuapkiconfiguration.h \
    client/qopcuatype.h

SOURCES += \
    client/qopcuaaddnodeitem.cpp \
    client/qopcuaaddreferenceitem.cpp \
    client/qopcuaapplicationdescription.cpp \
    client/qopcuaapplicationidentity.cpp \
    client/qopcuaapplicationrecorddatatype.cpp \
    client/qopcuaargument.cpp \
    client/qopcuaattributeoperand.cpp \
    client/qopcuaauthenticationinformation.cpp \
    client/qopcuaaxisinformation.cpp \
    client/qopcuabackend.cpp \
    client/qopcuabinarydataencoding.cpp \
    client/qopcuabrowsepathtarget.cpp \
    client/qopcuabrowserequest.cpp \
    client/qopcuaclient.cpp \
    client/qopcuaclientimpl.cpp \
    client/qopcuaclientprivate.cpp \
    client/qopcuacomplexnumber.cpp \
    client/qopcuacontentfilterelement.cpp \
    client/qopcuacontentfilterelementresult.cpp \
    client/qopcuadeletereferenceitem.cpp \
    client/qopcuadoublecomplexnumber.cpp \
    client/qopcuaelementoperand.cpp \
    client/qopcuaendpointdescription.cpp \
    client/qopcuaerrorstate.cpp \
    client/qopcuaeuinformation.cpp \
    client/qopcuaeventfilterresult.cpp \
    client/qopcuaexpandednodeid.cpp \
    client/qopcuaextensionobject.cpp \
    client/qopcualiteraloperand.cpp \
    client/qopcualocalizedtext.cpp \
    client/qopcuamonitoringparameters.cpp \
    client/qopcuamultidimensionalarray.cpp \
    client/qopcuanode.cpp \
    client/qopcuanodecreationattributes.cpp \
    client/qopcuanodeids.cpp \
    client/qopcuanodeimpl.cpp \
    client/qopcuapkiconfiguration.cpp \
    client/qopcuaqualifiedname.cpp \
    client/qopcuarange.cpp \
    client/qopcuareaditem.cpp \
    client/qopcuareadresult.cpp \
    client/qopcuareferencedescription.cpp \
    client/qopcuarelativepathelement.cpp \
    client/qopcuasimpleattributeoperand.cpp \
    client/qopcuatype.cpp \
    client/qopcuausertokenpolicy.cpp \
    client/qopcuawriteitem.cpp \
    client/qopcuawriteresult.cpp \
    client/qopcuaxvalue.cpp \

HEADERS += \
    client/qopcuaaddnodeitem.h \
    client/qopcuaaddreferenceitem.h \
    client/qopcuaapplicationdescription.h \
    client/qopcuaapplicationidentity.h \
    client/qopcuaapplicationrecorddatatype.h \
    client/qopcuaargument.h \
    client/qopcuaattributeoperand.h \
    client/qopcuaauthenticationinformation.h \
    client/qopcuaaxisinformation.h \
    client/qopcuabackend_p.h \
    client/qopcuabinarydataencoding.h \
    client/qopcuabrowsepathtarget.h \
    client/qopcuabrowserequest.h \
    client/qopcuaclient_p.h \
    client/qopcuaclientimpl_p.h \
    client/qopcuacomplexnumber.h \
    client/qopcuacontentfilterelement.h \
    client/qopcuacontentfilterelementresult.h \
    client/qopcuadeletereferenceitem.h \
    client/qopcuadoublecomplexnumber.h \
    client/qopcuaelementoperand.h \
    client/qopcuaendpointdescription.h \
    client/qopcuaerrorstate.h \
    client/qopcuaeuinformation.h \
    client/qopcuaeventfilterresult.h \
    client/qopcuaexpandednodeid.h \
    client/qopcuaextensionobject.h \
    client/qopcualiteraloperand.h \
    client/qopcualocalizedtext.h \
    client/qopcuamonitoringparameters.h \
    client/qopcuamonitoringparameters_p.h \
    client/qopcuamultidimensionalarray.h \
    client/qopcuanode_p.h \
    client/qopcuanodecreationattributes.h \
    client/qopcuanodecreationattributes_p.h \
    client/qopcuanodeids.h \
    client/qopcuanodeimpl_p.h \
    client/qopcuapkiconfiguration.h \
    client/qopcuaqualifiedname.h \
    client/qopcuarange.h \
    client/qopcuareaditem.h \
    client/qopcuareadresult.h \
    client/qopcuareferencedescription.h \
    client/qopcuarelativepathelement.h \
    client/qopcuasimpleattributeoperand.h \
    client/qopcuausertokenpolicy.h \
    client/qopcuawriteitem.h \
    client/qopcuawriteresult.h \
    client/qopcuaxvalue.h \

qtConfig(gds) {
    # Only added for platforms that have OpenSSL available
    QT_FOR_CONFIG += core-private
    qtConfig(ssl):!darwin:!winrt {
        PUBLIC_HEADERS += client/qopcuagdsclient.h
        SOURCES += client/qopcuagdsclient.cpp
        HEADERS += client/qopcuagdsclient_p.h
    }
}
