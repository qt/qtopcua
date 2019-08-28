OPEN62541_SOURCES += $$PWD/open62541/open62541.c
HEADERS += $$PWD/open62541/open62541.h
INCLUDEPATH += $$PWD/open62541

win32-msvc: LIBS += ws2_32.lib
win32-g++: LIBS += -lws2_32

win32-msvc|winrt: {
    SOURCES += $$OPEN62541_SOURCES
} else {
    OPEN62541_CFLAGS += -Wno-unused-parameter -Wno-unused-function -Wno-format -Wno-strict-aliasing -Wno-unused-result -std=c99

    # The open62541 source code produces lots of warnings.
    # This custom compiler will disable these warnings just for open62541.c
    open62541_compiler.commands = $$QMAKE_CC -c $(CFLAGS) $$OPEN62541_CFLAGS $(INCPATH) ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
    open62541_compiler.dependency_type = TYPE_C
    open62541_compiler.output = ${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_BASE}$${first(QMAKE_EXT_OBJ)}
    open62541_compiler.input += OPEN62541_SOURCES
    open62541_compiler.name = compiling ${QMAKE_FILE_IN}
    silent: open62541_compiler.commands = @echo compiling ${QMAKE_FILE_IN} && $$open62541_compiler.commands
    QMAKE_EXTRA_COMPILERS += open62541_compiler
}
