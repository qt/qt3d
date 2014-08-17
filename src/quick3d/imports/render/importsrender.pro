CXX_MODULE = qml
TARGET = quick3drenderplugin
TARGETPATH = Qt3D/Render

QT += core-private qml qml-private 3dcore 3drenderer 3dquick 3dquickrenderer-private

HEADERS += \
    qt3dquick3drenderplugin.h

SOURCES += \
    qt3dquick3drenderplugin.cpp

OTHER_FILES += qmldir

load(qml_plugin)
