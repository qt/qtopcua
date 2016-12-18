TEMPLATE = app
TARGET = qml-browser
QT += quick qml opcua

# Input
SOURCES += src/main.cpp

RESOURCES += \
    resources.qrc

OTHER_FILES += qml/*

QMAKE_RPATHDIR += $$ROOT_BUILD_DIR/lib

DISTFILES += \
    qml/OPCNode.qml \
    qml/Line.qml \
    qml/NodeText.qml
