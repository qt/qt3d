TARGET  = qsceneobj
include(../../qpluginbase.pri)

HEADERS += qglobjscene.h \
           qglobjscenehandler.h
SOURCES += main.cpp \
           qglobjscene.cpp \
           qglobjscenehandler.cpp
CONFIG += qt3d
QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/sceneformats
target.path += $$[QT_INSTALL_PLUGINS]/sceneformats
INSTALLS += target
