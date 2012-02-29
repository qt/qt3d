TEMPLATE = app
TARGET = moon
CONFIG += qt warn_on
QT += qml quick 3dquick

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

OTHER_FILES += \
    mt.qml \
    moon.rc \
    $$QML_INFRA_FILES

RC_FILE = moon.rc
