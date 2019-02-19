TEMPLATE = subdirs
SUBDIRS +=  qopcuaclient connection

QT_FOR_CONFIG += opcua-private

# only build declarative tests if at least one backend was built
qtConfig(open62541)|qtConfig(uacpp) {
    SUBDIRS += declarative
}
