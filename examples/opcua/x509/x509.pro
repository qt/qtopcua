QT += opcua
QT -= gui
CONFIG += c++11
DEPENDPATH += INCLUDEPATH

SOURCES += main.cpp

#install
target.path = $$[QT_INSTALL_EXAMPLES]/opcua/x509
INSTALLS += target
