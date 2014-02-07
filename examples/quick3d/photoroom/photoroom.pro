TEMPLATE = app
TARGET = photoroom
QT += qml quick 3dquick

SOURCES += main.cpp

ICON_FILE = ../icon.png

QML_FILES = \
    qml/desktop.qml \
    qml/PhotoPane.qml \
    qml/PhotoRoom.qml

QML_INFRA_FILES = \
    $$QML_FILES \
    qml/background.jpg \
    qml/basket.jpg \
    qml/niagara_falls.jpg \
    qml/place.jpg \
    qml/qtlogo.png \
    qml/woman.jpg

QML_MESHES_FILES = \
    qml/meshes/photopane.obj

CATEGORY = examples
include(../../../pkg.pri)

OTHER_FILES += \
    photoroom.rc \
    $$QML_INFRA_FILES

RC_FILE = photoroom.rc
