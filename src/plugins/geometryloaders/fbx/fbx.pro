TARGET = fbxgeometryloader
QT += core-private 3dcore 3dcore-private 3drender 3drender-private

HEADERS += \
    fbxgeometryloader.h \

SOURCES += \
    main.cpp \
    fbxgeometryloader.cpp \

DISTFILES += \
    fbx.json

include($$OUT_PWD/../qtgeometryloaders-config.pri)
QMAKE_USE += fbx

PLUGIN_TYPE = geometryloaders
PLUGIN_CLASS_NAME = fbxGeometryLoaderPlugin
load(qt_plugin)
