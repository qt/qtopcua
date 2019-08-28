TARGET = tst_security

QT += testlib opcua network
QT -= gui
CONFIG += testcase
RESOURCES += certs.qrc

SOURCES += \
    tst_security.cpp

HEADERS += \
    $$PWD/../../common/backend_environment.h

INCLUDEPATH += \
    $$PWD/../../common
