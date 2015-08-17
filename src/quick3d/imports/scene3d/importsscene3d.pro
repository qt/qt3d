CXX_MODULE = qml
TARGET = qtquickscene3dplugin
TARGETPATH = QtQuick/Scene3D

QT += qml quick 3dcore 3drenderer 3dinput

HEADERS += \
    qtquickscene3dplugin.h \
    scene3dlogging_p.h \
    scene3ditem_p.h

SOURCES += \
    qtquickscene3dplugin.cpp \
    scene3ditem.cpp \
    scene3dlogging.cpp

OTHER_FILES += qmldir

load(qml_plugin)
