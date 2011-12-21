TEMPLATE = app
TARGET = flickr3d
QT += declarative quick
CONFIG += qt warn_on

SOURCES += main.cpp

INSTALL_DIRS = qml
mt: INSTALL_FILES = mt.qml

OTHER_FILES += \
    flickr3d.rc

RC_FILE = flickr3d.rc

