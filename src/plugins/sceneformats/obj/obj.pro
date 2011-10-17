TARGET  = qsceneobj
include(../../qpluginbase.pri)

HEADERS += qglobjscene.h \
           qglobjscenehandler.h
SOURCES += main.cpp \
           qglobjscene.cpp \
           qglobjscenehandler.cpp

QT += qt3d

DESTDIR = $$QT.qt3d.plugins/sceneformats
target.path = $$[QT_INSTALL_PLUGINS]/sceneformats
INSTALLS += target
