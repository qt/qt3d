TEMPLATE = app
TARGET = shaders
CONFIG += qt warn_on
QT += declarative quick

SOURCES += main.cpp

INSTALL_DIRS = qml
mt: INSTALL_FILES = mt.qml

OTHER_FILES += \
    shaders.rc

RC_FILE = shaders.rc
