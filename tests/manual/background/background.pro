TEMPLATE = app
TARGET = tst_background
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
    $$QML_INFRA_FILES
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
