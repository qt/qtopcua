TEMPLATE = app
TARGET = tst_opcua
CONFIG += warn_on qmltestcase
SOURCES += tst_opcua.cpp
HEADERS += $$PWD/../../common/backend_environment.h
INCLUDEPATH += $$PWD/../../common
IMPORTPATH += $$PWD/../../../src/plugins/declarative

# This tries to check if the server has security support
QT += opcua_private
qtConfig(mbedtls): DEFINES += SERVER_SUPPORTS_SECURITY
