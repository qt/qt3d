CXX_MODULE = qml
TARGET = quick3dphysicsplugin
TARGETPATH = Qt3D/Physics
IMPORT_VERSION = 2.3

QT += core-private qml 3dcore 3dphysics 3dphysics-private

# Qt3D is free of Q_FOREACH - make sure it stays that way:
DEFINES += QT_NO_FOREACH

OTHER_FILES += qmldir

HEADERS += \
    qt3dquick3dphysicsplugin.h

SOURCES += \
    qt3dquick3dphysicsplugin.cpp

load(qml_plugin)
