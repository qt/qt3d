TEMPLATE = app
TARGET = tst_background
CONFIG += qt warn_on
QT += qml quick 3dquick

SOURCES += main.cpp

ICON_FILE = ../icon.png

QML_FILES = \
    qml/desktop.qml \
    qml/TestBackground.qml

QML_INFRA_FILES = \
    $$QML_FILES \
    qml/sky.png

CATEGORY = manual
include(../../../pkg.pri)

OTHER_FILES += \
    mt.qml \
    $$QML_INFRA_FILES
