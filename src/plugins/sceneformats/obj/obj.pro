TARGET  = qsceneobj
QT += 3d

load(qt_plugin)

include(../../qpluginbase.pri)

HEADERS += qglobjscene.h \
           qglobjscenehandler.h \
           obj_plugin.h
SOURCES += \
           qglobjscene.cpp \
           qglobjscenehandler.cpp \
           obj_plugin.cpp

OTHER_FILES += \
           obj_scene_plugin.json

DESTDIR = $$QT.3d.plugins/sceneformats
target.path = $$[QT_INSTALL_PLUGINS]/sceneformats
INSTALLS += target
