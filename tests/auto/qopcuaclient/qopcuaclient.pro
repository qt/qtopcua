TARGET = tst_qopcuaclient

QT += testlib opcua
QT -= gui
CONFIG += testcase

SOURCES += \
    tst_client.cpp

HEADERS += \
    $$PWD/../../common/backend_environment.h

INCLUDEPATH += \
    $$PWD/../../common

RESOURCES += data.qrc

# This tries to check if the server supports security
QT += opcua_private
qtConfig(mbedtls): DEFINES += SERVER_SUPPORTS_SECURITY
