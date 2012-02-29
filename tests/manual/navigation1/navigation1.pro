TEMPLATE = app
TARGET = tst_navigation1
CONFIG += qt warn_on

QT += 3dquick

SOURCES += main.cpp

ICON_FILE = ../icon.png

QML_FILES = \
    qml/desktop.qml \
    qml/TestNavigation1.qml

QML_INFRA_FILES = \
    $$QML_FILES

QML_MESHES_FILES = \
    qml/meshes/monkey.3ds

CATEGORY = manual
include(../../../pkg.pri)

OTHER_FILES += \
    mt.qml \
    $$QML_INFRA_FILES
