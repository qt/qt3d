TEMPLATE = app
TARGET = sphere
CONFIG += qt warn_on
QT += declarative quick

SOURCES += main.cpp

ICON_FILE = ../icon.png

QML_FILES = \
    qml/desktop.qml \
    qml/Sphere.qml

QML_INFRA_FILES = \
    $$QML_FILES

CATEGORY = examples
include(../../../pkg.pri)

INSTALL_DIRS = qml
mt: INSTALL_FILES = mt.qml

OTHER_FILES += \
    sphere.rc \
    $$QML_INFRA_FILES

RC_FILE = sphere.rc
