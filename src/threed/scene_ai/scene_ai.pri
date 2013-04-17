INCLUDEPATH += $$PWD
VPATH += $$PWD
HEADERS += \
    scene_ai/qaiscenehandler_p.h \
    scene_ai/qaiscene_p.h \
    scene_ai/qaimesh_p.h \
    scene_ai/qailoader_p.h \
    scene_ai/ailoaderiosystem_p.h \
    scene_ai/ailoaderiostream_p.h
SOURCES += \
    scene_ai/qaiscenehandler.cpp \
    scene_ai/qaiscene.cpp \
    scene_ai/qaimesh.cpp \
    scene_ai/qailoader.cpp \
    scene_ai/ailoaderiosystem.cpp \
    scene_ai/ailoaderiostream.cpp


# suppress warnings about std::copy using unsafe parameters
win32: DEFINES +=_SCL_SECURE_NO_WARNINGS
include(../../../3rdparty/assimp/assimp.pri)
