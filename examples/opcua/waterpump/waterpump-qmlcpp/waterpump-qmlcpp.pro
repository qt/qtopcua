QT += quick opcua
CONFIG += c++11

SOURCES += main.cpp \
    opcuamachinebackend.cpp

RESOURCES += qml.qrc

HEADERS += \
    opcuamachinebackend.h

#install
target.path = $$[QT_INSTALL_EXAMPLES]/opcua/waterpump/waterpump-qmlcpp
INSTALLS += target
