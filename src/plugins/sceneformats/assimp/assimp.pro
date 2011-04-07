TARGET = qsceneai
include(../../qpluginbase.pri)
HEADERS += qailoader.h \
    qaiscene.h \
    qaiscenehandler.h \
    qaimesh.h \
    ailoaderiostream.h \
    ailoaderiosystem.h
SOURCES += main.cpp \
    qailoader.cpp \
    qaiscene.cpp \
    qaiscenehandler.cpp \
    qaimesh.cpp \
    ailoaderiostream.cpp \
    ailoaderiosystem.cpp

package {
    LIBS += -L$$PWD/../threed -lQt3D
    INCLUDEPATH += ../../../../include
} else {
    CONFIG += qt3d
}

system_ai {
    !isEmpty(QMAKE_INCDIR_AI):INCLUDEPATH += $$QMAKE_INCDIR_AI
    !isEmpty(QMAKE_LIBDIR_AI):LIBS += -L$$QMAKE_LIBDIR_AI

    !isEmpty(QMAKE_LIBS_AI):LIBS += -l$$QMAKE_LIBS_AI
    else {
        win32:LIBS += -llibai-1_3
        else:LIBS += -lai
    }
} else {
    include(../../../../3rdparty/assimp/assimp.pri)
}

target.path += $$[QT_INSTALL_PLUGINS]/sceneformats
INSTALLS += target
