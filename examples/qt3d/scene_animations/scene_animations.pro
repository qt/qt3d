TEMPLATE = app
TARGET = scene_animations
CONFIG += qt warn_on

QT += 3d

include(../../../pkg.pri)

SOURCES = scene_animations.cpp \
    scene_animations_main.cpp
HEADERS = scene_animations.h
RESOURCES += scene_animations.qrc

OTHER_FILES += \
    scene_animations.rc

RC_FILE = scene_animations.rc
