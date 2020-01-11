CXX_MODULE = qml
TARGET = quick3drenderplugin
TARGETPATH = Qt3D/Render
IMPORT_VERSION = 2.$$QT_MINOR_VERSION
DEFINES += BUILD_QT3D_MODULE

QT += core-private qml qml-private 3dcore 3drender 3drender-private 3dquick 3dquick-private 3dquickrender-private

HEADERS += \
    qt3dquick3drenderplugin.h

SOURCES += \
    qt3dquick3drenderplugin.cpp

OTHER_FILES += qmldir

load(qml_plugin)
