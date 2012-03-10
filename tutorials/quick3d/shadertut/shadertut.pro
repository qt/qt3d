TEMPLATE = app
TARGET = shadertut
CONFIG += qt warn_on
QT += qml quick

SOURCES += main.cpp

ICON_FILE = ../icon.png

QML_FILES = \
    qml/TutorialTeapot.qml \
    qml/shader-tutorial-1-textures.qml \
    qml/shader-tutorial-3-transformed-coordinates.qml \
    qml/shader-tutorial-varying.qml \
    qml/shader-tutorial-2-vertex-shader.qml \
    qml/shader-tutorial-lighting.qml \
    qml/shader-tutorial.qml

QML_INFRA_FILES = \
    $$QML_FILES \
    qml/basket.jpg

QML_MESHES_FILES = \
    qml/meshes/teapot.bez

CATEGORY = examples
include(../../../pkg.pri)

INSTALL_DIRS = qml
mt: INSTALL_FILES = mt.qml

OTHER_FILES += \
    mt.qml \
    basket_qml.rc \
    $$QML_INFRA_FILES

RC_FILE = basket_qml.rc

RESOURCES += \
    basket.qrc

