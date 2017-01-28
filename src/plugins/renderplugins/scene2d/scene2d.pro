TARGET = scene2d
QT += core-private 3dcore 3dcore-private 3drender 3drender-private 3dextras 3dquickrender 3dquickrender-private

# Qt3D is free of Q_FOREACH - make sure it stays that way:
DEFINES += QT_NO_FOREACH

HEADERS += \
    scene2dplugin.h
#    scene2dnode_p.h

SOURCES += \
    main.cpp \
    scene2dplugin.cpp
#    scene2dnode.cpp

DISTFILES += \
    scene2dplugin.json

PLUGIN_TYPE = renderplugins
PLUGIN_CLASS_NAME = Scene2DPlugin
load(qt_plugin)
