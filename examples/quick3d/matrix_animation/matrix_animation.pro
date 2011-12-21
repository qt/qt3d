TEMPLATE = app
TARGET = matrix_animation
CONFIG += qt warn_on
QT += declarative quick

SOURCES += main.cpp

INSTALL_DIRS = qml
mt: INSTALL_FILES = mt.qml

OTHER_FILES += \
    matrix_animation.rc

RC_FILE = matrix_animation.rc
