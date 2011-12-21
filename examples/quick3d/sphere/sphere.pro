TEMPLATE = app
TARGET = sphere
CONFIG += qt warn_on
QT += declarative quick

SOURCES += main.cpp

INSTALL_DIRS = qml
mt: INSTALL_FILES = mt.qml

OTHER_FILES += \
    sphere.rc

RC_FILE = sphere.rc
