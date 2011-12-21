TEMPLATE = app
TARGET = cube_qml
QT += declarative quick
CONFIG += qt warn_on

SOURCES += main.cpp

INSTALL_DIRS = qml
mt: INSTALL_FILES = mt.qml

OTHER_FILES += \
    cube_qml.rc

RC_FILE = cube_qml.rc
