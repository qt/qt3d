TARGET  = qscenebezier
include(../../qpluginbase.pri)

HEADERS += qglbezierscene.h \
           qglbezierscenehandler.h
SOURCES += main.cpp \
           qglbezierscene.cpp \
           qglbezierscenehandler.cpp

package {
    LIBS += -L$$PWD/../../../threed -lQt3D
    INCLUDEPATH += ../../../../include
} else {
    CONFIG += qt3d
    DESTDIR = $$[QT_INSTALL_PLUGINS]/sceneformats
}

target.path += $$[QT_INSTALL_PLUGINS]/sceneformats
INSTALLS += target
