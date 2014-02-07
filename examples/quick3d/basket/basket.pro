TEMPLATE = app
TARGET = basket_qml
QT += qml quick 3dquick

SOURCES += main.cpp

ICON_FILE = ../icon.png

QML_FILES = \
    qml/desktop.qml \
    qml/Basket.qml

QML_INFRA_FILES = \
    $$QML_FILES \
    qml/basket.jpg

QML_MESHES_FILES = \
    qml/meshes/basket.bez

CATEGORY = examples
include(../../../pkg.pri)

OTHER_FILES += \
    basket_qml.rc \
    $$QML_INFRA_FILES

RC_FILE = basket_qml.rc
