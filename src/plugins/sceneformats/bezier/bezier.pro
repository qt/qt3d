TARGET  = qscenebezier
QT += 3d

load(qt_plugin)

include(../../qpluginbase.pri)

HEADERS += qglbezierscene.h \
           qglbezierscenehandler.h \
           bezier_plugin.h
SOURCES += \
           qglbezierscene.cpp \
           qglbezierscenehandler.cpp \
           bezier_plugin.cpp

OTHER_FILES += \
            bezier_scene_plugin.json

DESTDIR = $$QT.3d.plugins/sceneformats
target.path = $$[QT_INSTALL_PLUGINS]/sceneformats
INSTALLS += target
