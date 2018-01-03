QT += quick opcua
CONFIG += c++11
TARGET = qml-hmi

SOURCES += main.cpp \
    opcuamachinebackend.cpp

RESOURCES += qml.qrc

HEADERS += \
    opcuamachinebackend.h

#install
target.path = $$[QT_INSTALL_EXAMPLES]/opcua/waterpump/qml-hmi
INSTALLS += target
