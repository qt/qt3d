CXX_MODULE = qml
TARGET = quick3dcollisionplugin
TARGETPATH = Qt3D/Collision

QT += core-private qml 3dcore 3dcollision

OTHER_FILES += qmldir

load(qml_plugin)

HEADERS += \
    qt3dquick3dcollisionplugin.h

SOURCES += \
    qt3dquick3dcollisionplugin.cpp
