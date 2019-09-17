QT += opcua widgets
CONFIG += c++11
DEPENDPATH += INCLUDEPATH

SOURCES += main.cpp \
    mainwindow.cpp \
    opcuamodel.cpp \
    treeitem.cpp \
    certificatedialog.cpp

#install
target.path = $$[QT_INSTALL_EXAMPLES]/opcua/opcuaviewer
INSTALLS += target

HEADERS += \
    mainwindow.h \
    opcuamodel.h \
    treeitem.h \
    certificatedialog.h

FORMS += \
    certificatedialog.ui \
    mainwindow.ui \
