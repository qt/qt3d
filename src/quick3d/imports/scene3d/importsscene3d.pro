CXX_MODULE = qml
TARGET = qtquickscene3dplugin
TARGETPATH = QtQuick/Scene3D

QT += qml quick 3dcore 3drender 3dinput

HEADERS += \
    qtquickscene3dplugin.h \
    scene3dlogging_p.h \
    scene3ditem_p.h \
    scene3dcleaner_p.h \
    scene3drenderer_p.h \
    scene3dsgnode_p.h \
    scene3dsgmaterialshader_p.h \
    scene3dsgmaterial_p.h

SOURCES += \
    qtquickscene3dplugin.cpp \
    scene3ditem.cpp \
    scene3dlogging.cpp \
    scene3dcleaner.cpp \
    scene3drenderer.cpp \
    scene3dsgnode.cpp \
    scene3dsgmaterialshader.cpp \
    scene3dsgmaterial.cpp

OTHER_FILES += qmldir

load(qml_plugin)
