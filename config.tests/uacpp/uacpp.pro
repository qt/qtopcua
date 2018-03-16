SOURCES += main.cpp

CONFIG(debug, debug|release): \
    LIBS += $$LIBS_DEBUG
else: \
    LIBS += $$LIBS_RELEASE
