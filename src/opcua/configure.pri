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

defineTest(qtConfLibrary_uacpp) {
    !qtConfLibrary_inline($$1, $$2): \
        return(false)

    # The Windows SDK ships its dependencies and locates the libraries outside of lib
    win32 {
        lessThan(QMAKE_MSC_VER, 1900) {
            qtLog("UaCpp SDK requires at least Visual Studio 2015")
            return(false)
        }

        input = $$eval($${2}.alias)
        !isEmpty(config.input.$${input}.prefix) {
            prefix = $$val_escape(config.input.$${input}.prefix)

            $${1}.libs += -L$${prefix}/third-party/win32/vs2015/openssl/out32dll \
                          -L$${prefix}/third-party/win32/vs2015/libxml2/out32dll
            export($${1}.libs)
        } else {
            qtLog("No UACPP_PREFIX specified; relying on user-provided library path.")
        }
    }

    inc = $$eval($${1}.includedir)
    $${1}.includedir += $$inc/uabase $$inc/uaclient $$inc/uastack $$inc/uapki
    export($${1}.includedir)
    return(true)
}
