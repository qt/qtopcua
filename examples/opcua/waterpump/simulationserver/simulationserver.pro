TEMPLATE = app
CONFIG += c++11 console
QT += opcua opcua-private
QT -= gui

# Installed example package

MODULE_SOURCES=../../../../../$$QT_VERSION/Src/qtopcua/

!exists($$MODULE_SOURCES/src/plugins/opcua/open62541/qopen62541utils.cpp):{
  # Build from source tree
  MODULE_SOURCES=$$PWD/../../../../
}

!exists($$MODULE_SOURCES/src/plugins/opcua/open62541/qopen62541utils.cpp):{
  # Build from source tree
  MODULE_SOURCES=$$QTOPCUA_SOURCE_DIR
}

!exists($$MODULE_SOURCES/src/plugins/opcua/open62541/qopen62541utils.cpp):{
  error("Unable to build the server, the Qt OPC UA module source is not available. Set QTOPCUA_SOURCE_DIR to the location of the Qt OPC UA sources.")
}

INCLUDEPATH += $$MODULE_SOURCES/src/plugins/opcua/open62541
DEPENDPATH += INCLUDEPATH

qtConfig(open62541):!qtConfig(system-open62541) {
    include($$MODULE_SOURCES/src/3rdparty/open62541.pri)
} else {
    DEFINES += USE_SYSTEM_OPEN62541
    QMAKE_LIBS_PRIVATE += -lopen62541

    # The system open62541 library might require OpenSSL
    qtConfig(open62541-security): QMAKE_LIBS_PRIVATE += -lcrypto
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
