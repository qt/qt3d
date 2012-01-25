TEMPLATE = app
TARGET = moon
CONFIG += qt warn_on
QT += declarative quick

SOURCES += main.cpp

ICON_FILE = ../icon.png

QML_FILES = \
    qml/desktop.qml \
    qml/Moon.qml

QML_INFRA_FILES = \
    $$QML_FILES \
    qml/moon-texture.jpg

CATEGORY = examples
include(../../../pkg.pri)

INSTALL_DIRS = qml
mt: INSTALL_FILES = mt.qml

OTHER_FILES += \
    moon.rc \
    $$QML_INFRA_FILES

RC_FILE = moon.rc
