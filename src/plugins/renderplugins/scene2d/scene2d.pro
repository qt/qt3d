TARGET = scene2d
QT += core-private 3dcore 3dcore-private 3drender 3drender-private 3dextras 3dquickscene2d 3dquickscene2d-private
DEFINES += BUILD_QT3D_MODULE

HEADERS += \
    scene2dplugin.h

SOURCES += \
    main.cpp \
    scene2dplugin.cpp

DISTFILES += \
    scene2dplugin.json

PLUGIN_TYPE = renderplugins
PLUGIN_CLASS_NAME = Scene2DPlugin
load(qt_plugin)
