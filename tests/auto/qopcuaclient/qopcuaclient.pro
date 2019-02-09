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
