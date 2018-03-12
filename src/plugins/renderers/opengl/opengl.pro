TARGET = openglrenderer

include(opengl.pri)

SOURCES += \
    main.cpp

PLUGIN_TYPE = renderers
PLUGIN_CLASS_NAME = OpenGLRendererPlugin
load(qt_plugin)
