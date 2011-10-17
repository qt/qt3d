TARGET  = qscenebezier
include(../../qpluginbase.pri)

HEADERS += qglbezierscene.h \
           qglbezierscenehandler.h
SOURCES += main.cpp \
           qglbezierscene.cpp \
           qglbezierscenehandler.cpp

QT += qt3d

DESTDIR = $$QT.qt3d.plugins/sceneformats
target.path = $$[QT_INSTALL_PLUGINS]/sceneformats
INSTALLS += target
