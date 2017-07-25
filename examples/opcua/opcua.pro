TEMPLATE = subdirs

SUBDIRS = accontrol

qtHaveModule(quick): SUBDIRS += qml-browser

