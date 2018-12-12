SOURCES += $$PWD/open62541/open62541.c
HEADERS += $$PWD/open62541/open62541.h
INCLUDEPATH += $$PWD/open62541

win32-msvc: LIBS += ws2_32.lib
win32-g++: LIBS += -lws2_32
