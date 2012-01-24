TEMPLATE = app
TARGET = basket_qml
CONFIG += qt warn_on
QT += declarative quick

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

#INSTALL_DIRS = qml
#mt: INSTALL_FILES = mt.qml

OTHER_FILES += \
    mt.qml \
    basket_qml.rc \
    $$QML_INFRA_FILES

RC_FILE = basket_qml.rc

RESOURCES += \
    basket.qrc

