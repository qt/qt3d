TARGET  = qsceneobj
include(../../qpluginbase.pri)

HEADERS += qglobjscene.h \
           qglobjscenehandler.h
SOURCES += main.cpp \
           qglobjscene.cpp \
           qglobjscenehandler.cpp

QT += 3d

DESTDIR = $$QT.3d.plugins/sceneformats
target.path = $$[QT_INSTALL_PLUGINS]/sceneformats
INSTALLS += target
