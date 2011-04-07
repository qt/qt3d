TARGET  = qsceneobj
include(../../qpluginbase.pri)

HEADERS += qglobjscene.h \
           qglobjscenehandler.h
SOURCES += main.cpp \
           qglobjscene.cpp \
           qglobjscenehandler.cpp

package {
    LIBS += -L$$PWD/../threed -lQt3D
    INCLUDEPATH += ../../../../include
} else {
    CONFIG += qt3d
}

target.path += $$[QT_INSTALL_PLUGINS]/sceneformats
INSTALLS += target
