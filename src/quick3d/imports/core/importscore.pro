CXX_MODULE = qml
TARGET = quick3dcoreplugin
TARGETPATH = Qt3D

QT += qml 3dcore 3dcore-private 3dquick 3dquick-private

HEADERS += \
    qt3dquick3dcoreplugin.h

SOURCES += \
    qt3dquick3dcoreplugin.cpp

OTHER_FILES += qmldir

load(qml_plugin)
