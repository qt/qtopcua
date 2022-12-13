QT += opcua widgets
CONFIG += c++11
DEPENDPATH += INCLUDEPATH

SOURCES += main.cpp \
    mainwindow.cpp \
    opcuamodel.cpp \
    treeitem.cpp \
    certificatedialog.cpp

#install
target.path = $$[QT_INSTALL_EXAMPLES]/opcua/opcuaviewer
INSTALLS += target

HEADERS += \
    mainwindow.h \
    opcuamodel.h \
    treeitem.h \
    certificatedialog.h

FORMS += \
    certificatedialog.ui \
    mainwindow.ui \

pki.files = \
    pki/own/certs/opcuaviewer.der \
    pki/own/private/opcuaviewer.pem \
    pki/trusted/certs/3d8ec65c47524d6ad67bed912c19a895.der \
    pki/trusted/certs/ca.der \
    pki/trusted/certs/open62541-testserver.der \
    pki/trusted/crl/ca.crl.pem

pki.prefix = /

RESOURCES += pki
