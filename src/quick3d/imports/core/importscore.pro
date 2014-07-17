CXX_MODULE = qml
TARGET = quick3dcoreplugin
TARGETPATH = Qt3D

QT += qml 3dcore 3dquick

HEADERS += \
    qt3dquick3dcoreplugin.h

SOURCES += \
    qt3dquick3dcoreplugin.cpp

OTHER_FILES += qmldir

load(qml_plugin)
