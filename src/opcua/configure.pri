# Add OPC UA SDKs using environment variable

defineTest(qtConfLibrary_open62541) {
    inc = $$getenv("QTOPCUA_OPEN62541_INCLUDE_PATH")
    lib = $$getenv("QTOPCUA_OPEN62541_LIB_PATH")

    isEmpty(inc) {
        qtLog("$QTOPCUA_OPEN62541_INCLUDE_PATH is not set.")
        return(false)
    }

    isEmpty(lib) {
        qtLog("$QTOPCUA_OPEN62541_LIB_PATH is not set.")
        return(false)
    }

    $${1}.libs += -L$${lib}
    $${1}.includedir = $${inc}
    export($${1}.libs)
    export($${1}.includedir)
    return(true)
}
