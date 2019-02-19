TARGET = tst_connection

QT += testlib opcua
QT -= gui
CONFIG += testcase

SOURCES += \
    tst_connection.cpp

HEADERS += \
    $$PWD/../../common/backend_environment.h

INCLUDEPATH += \
    $$PWD/../../common
