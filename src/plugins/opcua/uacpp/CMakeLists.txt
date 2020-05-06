# Generated from uacpp.pro.

#####################################################################
## QUACppPlugin Plugin:
#####################################################################

qt_internal_add_plugin(QUACppPlugin
    OUTPUT_NAME uacpp_backend
    TYPE opcua
    SOURCES
        quacppbackend.cpp quacppbackend.h
        quacppclient.cpp quacppclient.h
        quacppnode.cpp quacppnode.h
        quacppplugin.cpp quacppplugin.h
        quacppsubscription.cpp quacppsubscription.h
        quacpputils.cpp quacpputils.h
        quacppvalueconverter.cpp quacppvalueconverter.h
    PUBLIC_LIBRARIES
        Qt::Core
        Qt::CorePrivate
        Qt::Network
        Qt::OpcUa
        Qt::OpcUaPrivate
        uacpp
)

#### Keys ignored in scope 1:.:.:uacpp.pro:<TRUE>:
# OTHER_FILES = "uacpp-metadata.json"

## Scopes:
#####################################################################

qt_extend_target(QUACppPlugin CONDITION WIN32
    DEFINES
        _UA_STACK_USE_DLL
)
