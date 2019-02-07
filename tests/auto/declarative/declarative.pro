TEMPLATE = app
TARGET = tst_opcua
CONFIG += warn_on qmltestcase
SOURCES += tst_opcua.cpp
HEADERS += $$PWD/../../common/backend_environment.h
INCLUDEPATH += $$PWD/../../common
IMPORTPATH += $$PWD/../../../src/plugins/declarative
