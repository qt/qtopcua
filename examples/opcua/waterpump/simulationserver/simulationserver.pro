TEMPLATE = app
CONFIG += c++11 console
QT += opcua opcua-private
QT -= gui

# Installed example package
greaterThan(QT_MAJOR_VERSION, 5) {
    MODULE_SOURCES=../../../../../$$QT_VERSION/Src/qtopcua/
} else {
    MODULE_SOURCES=../../../../../$$QT_VERSION/Automation/sources/qtopcua/
}

!exists($$MODULE_SOURCES):{
  # Build from source tree
  MODULE_SOURCES=$$PWD/../../../../
}

INCLUDEPATH += $$MODULE_SOURCES/src/plugins/opcua/open62541
DEPENDPATH += INCLUDEPATH

qtConfig(open62541):!qtConfig(system-open62541) {
    include($$MODULE_SOURCES/src/3rdparty/open62541.pri)
} else {
    QMAKE_USE_PRIVATE += open62541
}

SOURCES += main.cpp \
    simulationserver.cpp \
    $$MODULE_SOURCES/src/plugins/opcua/open62541/qopen62541utils.cpp \
    $$MODULE_SOURCES/src/plugins/opcua/open62541/qopen62541valueconverter.cpp

HEADERS += \
    simulationserver.h

#install
target.path = $$[QT_INSTALL_EXAMPLES]/opcua/waterpump/simulationserver
INSTALLS += target
