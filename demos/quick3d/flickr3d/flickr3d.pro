TEMPLATE = app
TARGET = flickr3d
QT += qml quick 3dquick

SOURCES += main.cpp
ICON_FILE = ../icon.png

QML_FILES = \
    qml/Flickr3d.qml \
    qml/desktop.qml \
    qml/RssModel.qml

QML_INFRA_FILES = \
    $$QML_FILES \
    qml/qtlogo.png \
    qml/default_sky_down.png \
    qml/default_sky_north.png \
    qml/default_sky_up.png \
    qml/default_sky_east.png \
    qml/default_sky_south.png \
    qml/default_sky_west.png \


CATEGORY = demos
include(../../../pkg.pri)

OTHER_FILES += \
    flickr3d.rc \
    $$QML_INFRA_FILES

RC_FILE = flickr3d.rc

