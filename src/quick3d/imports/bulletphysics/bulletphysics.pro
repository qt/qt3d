CXX_MODULE = qml
TARGET = quick3dbulletphysicsplugin
TARGETPATH = Qt3D/BulletPhysics

QT += core-private qml 3dcore 3dbulletphysics 3dquick

HEADERS += \
    qt3dquick3dbulletphysicsplugin.h

SOURCES += \
    qt3dquick3dbulletphysicsplugin.cpp

OTHER_FILES += qmldir

load(qml_plugin)
