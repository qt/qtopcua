TEMPLATE = app
TARGET = accontrol

QT += widgets opcua

QMAKE_RPATHDIR += $${ROOT_BUILD_DIR}/lib

# Input
HEADERS += \
    accontroltest.h
FORMS += \
    accontroltest.ui
SOURCES += main.cpp \
    accontroltest.cpp
