PUBLIC_HEADERS += \
    $$PWD/qopcuakeypair.h \
    $$PWD/qopcuax509certificatesigningrequest.h \
    $$PWD/qopcuax509distinguishedname.h \
    $$PWD/qopcuax509extensionextendedkeyusage.h \
    $$PWD/qopcuax509extension.h \
    $$PWD/qopcuax509extensionkeyusage.h \
    $$PWD/qopcuax509extensionsubjectalternativename.h \
    $$PWD/qopcuax509extensionbasicconstraints.h \

PRIVATE_HEADERS += \
    $$PWD/qopcuakeypair_p.h \
    $$PWD/openssl_symbols_p.h \
    $$PWD/qsslsocket_openssl11_symbols_p.h \
    $$PWD/qsslsocket_opensslpre11_symbols_p.h \
    $$PWD/qopcuax509certificatesigningrequest_p.h \
    $$PWD/qopcuax509utils_p.h \
    $$PWD/qopcuax509extension_p.h \

SOURCES += \
    $$PWD/qopcuax509certificatesigningrequest.cpp \
    $$PWD/qopcuax509distinguishedname.cpp \
    $$PWD/qopcuax509extensionextendedkeyusage.cpp \
    $$PWD/qopcuax509extension.cpp \
    $$PWD/qopcuax509extensionkeyusage.cpp \
    $$PWD/qopcuax509extensionsubjectalternativename.cpp \
    $$PWD/qopcuax509extensionbasicconstraints.cpp \
    $$PWD/openssl_symbols.cpp \
    $$PWD/qopcuakeypair.cpp \
    $$PWD/qopcuakeypair_openssl.cpp \
    $$PWD/qopcuax509certificatesigningrequest_openssl.cpp \
    $$PWD/qopcuax509utils.cpp \

