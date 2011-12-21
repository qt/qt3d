TEMPLATE = app
TARGET = photoroom
CONFIG += qt warn_on
QT += declarative quick

SOURCES += main.cpp

INSTALL_DIRS = qml
mt: INSTALL_FILES = mt.qml

OTHER_FILES += \
    photoroom.rc

RC_FILE = photoroom.rc
