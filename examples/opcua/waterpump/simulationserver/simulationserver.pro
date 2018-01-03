TEMPLATE = app
TARGET = simulationserver

INCLUDEPATH += \
               $$PWD/../../../../src/plugins/opcua/open62541

DEPENDPATH += INCLUDEPATH

CONFIG += c++11 console

QT += opcua

QMAKE_USE_PRIVATE += open62541

SOURCES += main.cpp \
    simulationserver.cpp \
    $$PWD/../../../../src/plugins/opcua/open62541/qopen62541utils.cpp

HEADERS += \
    simulationserver.h

#install
target.path = $$[QT_INSTALL_EXAMPLES]/opcua/waterpump/simulationserver
INSTALLS += target
