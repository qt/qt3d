CXX_MODULE = qml
TARGET = quick3dcoreplugin
TARGETPATH = Qt3D/Core
IMPORT_VERSION = 2.$$QT_MINOR_VERSION
DEFINES += BUILD_QT3D_MODULE

QT += core-private qml qml-private quick quick-private 3dcore 3dcore-private 3dquick 3dquick-private

HEADERS += \
    qt3dquick3dcoreplugin.h

SOURCES += \
    qt3dquick3dcoreplugin.cpp

OTHER_FILES += qmldir

load(qml_plugin)
