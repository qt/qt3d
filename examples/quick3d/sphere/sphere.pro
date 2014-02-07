TEMPLATE = app
TARGET = sphere
QT += qml quick 3dquick

SOURCES += main.cpp

ICON_FILE = ../icon.png

QML_FILES = \
    qml/desktop.qml \
    qml/Sphere.qml

QML_INFRA_FILES = \
    $$QML_FILES

CATEGORY = examples
include(../../../pkg.pri)

OTHER_FILES += \
    sphere.rc \
    $$QML_INFRA_FILES

RC_FILE = sphere.rc
