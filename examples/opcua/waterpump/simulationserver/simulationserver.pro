TEMPLATE = app

INCLUDEPATH += \
               $$PWD/../../../../src/plugins/opcua/open62541

DEPENDPATH += INCLUDEPATH

CONFIG += c++11 console

QT += opcua opcua-private

qtConfig(open62541):!qtConfig(system-open62541) {
    include($$PWD/../../../../src/3rdparty/open62541.pri)
} else {
    QMAKE_USE_PRIVATE += open62541
}

SOURCES += main.cpp \
    simulationserver.cpp \
    $$PWD/../../../../src/plugins/opcua/open62541/qopen62541utils.cpp \
    $$PWD/../../../../src/plugins/opcua/open62541/qopen62541valueconverter.cpp

HEADERS += \
    simulationserver.h

#install
target.path = $$[QT_INSTALL_EXAMPLES]/opcua/waterpump/simulationserver
INSTALLS += target
