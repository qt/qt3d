TARGET = openglrenderer

# We use QT_AUTOTEST_EXPORT to test the plug-ins, which needs QT_BUILDING_QT
DEFINES += QT_BUILDING_QT

include(opengl.pri)

SOURCES += \
    main.cpp

PLUGIN_TYPE = renderers
PLUGIN_CLASS_NAME = OpenGLRendererPlugin
load(qt_plugin)
