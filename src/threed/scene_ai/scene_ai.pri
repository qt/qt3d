INCLUDEPATH += $$PWD
VPATH += $$PWD
HEADERS += \
    scene_ai/qaianimation.h \
    scene_ai/qaianimation_p.h \
    scene_ai/qaiscenehandler.h \
    scene_ai/qaiscene.h \
    scene_ai/qaimesh.h \
    scene_ai/qailoader.h \
    scene_ai/ailoaderiosystem.h \
    scene_ai/ailoaderiostream.h
SOURCES += \
    scene_ai/qaianimation.cpp \
    scene_ai/qaiscenehandler.cpp \
    scene_ai/qaiscene.cpp \
    scene_ai/qaimesh.cpp \
    scene_ai/qailoader.cpp \
    scene_ai/ailoaderiosystem.cpp \
    scene_ai/ailoaderiostream.cpp


# suppress warnings about std::copy using unsafe parameters
win32: DEFINES +=_SCL_SECURE_NO_WARNINGS
include(../../../3rdparty/assimp/assimp.pri)
