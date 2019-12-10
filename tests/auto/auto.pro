TEMPLATE = subdirs
SUBDIRS +=  qopcuaclient connection clientSetupInCpp security

QT_FOR_CONFIG += opcua-private core-private

# only build declarative tests if at least one backend was built
qtHaveModule(qmltest):qtConfig(open62541)|qtConfig(uacpp) {
    SUBDIRS += declarative
}

qtConfig(ssl):!darwin:!winrt: SUBDIRS += x509
