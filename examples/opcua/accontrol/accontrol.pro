TEMPLATE = app
TARGET = accontrol

QT += widgets opcua

# Input
HEADERS += \
    accontroltest.h
FORMS += \
    accontroltest.ui
SOURCES += main.cpp \
    accontroltest.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/opcua/accontrol
INSTALLS += target
