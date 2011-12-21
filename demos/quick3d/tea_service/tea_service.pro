TEMPLATE = app
TARGET = tea_service
QT += declarative quick
CONFIG += qt warn_on

SOURCES += main.cpp

INSTALL_DIRS = qml
mt: INSTALL_FILES = mt.qml

OTHER_FILES += \
    tea_service.rc

RC_FILE = tea_service.rc
