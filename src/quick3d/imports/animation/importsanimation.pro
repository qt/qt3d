CXX_MODULE = qml
TARGET = quick3danimationplugin
TARGETPATH = Qt3D/Animation
IMPORT_VERSION = 2.$$QT_MINOR_VERSION
DEFINES += BUILD_QT3D_MODULE

QT += core-private qml qml-private 3dcore 3drender 3danimation 3dquick 3dquickrender 3dquickanimation 3dquick-private 3dquickanimation-private

HEADERS += \
    qt3dquick3danimationplugin.h

SOURCES += \
    qt3dquick3danimationplugin.cpp

OTHER_FILES += qmldir

load(qml_plugin)
