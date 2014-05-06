CXX_MODULE = qml
TARGET = quick3drenderplugin
TARGETPATH = Qt3D/Render

QT += qml 3dcore 3drenderer 3dquick 3dquickrenderer

HEADERS += \
    qt3dquick3drenderplugin.h

SOURCES += \
    qt3dquick3drenderplugin.cpp

OTHER_FILES += qmldir

load(qml_plugin)
