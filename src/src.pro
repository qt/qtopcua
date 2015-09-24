TEMPLATE = subdirs

SUBDIRS = opcua
opcua.subdir = opcua
opcua.target = sub-opcua

SUBDIRS += plugins
plugin.subdir = plugins
plugin.target = sub-plugins
plugins.depends = opcua
