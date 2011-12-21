TEMPLATE = app
TARGET = model_viewer
QT += declarative quick
CONFIG += qt warn_on

SOURCES += main.cpp

INSTALL_DIRS = qml
mt: INSTALL_FILES = mt.qml

OTHER_FILES += \
    model_viewer.rc

RC_FILE = model_viewer.rc
