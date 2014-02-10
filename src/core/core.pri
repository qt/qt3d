INCLUDEPATH += $$PWD

include(./transforms/transforms.pri)
include(./aspects/aspects.pri)
include(./jobs/jobs.pri)

HEADERS += \
    $$PWD/qt3dcore_global.h \
    $$PWD/node.h \
    $$PWD/nodevisitor.h \
    $$PWD/axisalignedboundingbox.h \
    $$PWD/sphere.h \
    $$PWD/bounds.h \
    $$PWD/component.h \
    $$PWD/window.h \
    $$PWD/qtickclock.h \
    $$PWD/entity.h \
    $$PWD/scene.h \
    $$PWD/qscheduler.h \
    $$PWD/cameracontroller.h \
    $$PWD/qchangearbiter.h

SOURCES += \
    $$PWD/node.cpp \
    $$PWD/nodevisitor.cpp \
    $$PWD/axisalignedboundingbox.cpp \
    $$PWD/sphere.cpp \
    $$PWD/bounds.cpp \
    $$PWD/component.cpp \
    $$PWD/window.cpp \
    $$PWD/qtickclock.cpp \
    $$PWD/entity.cpp \
    $$PWD/scene.cpp \
    $$PWD/qscheduler.cpp \
    $$PWD/cameracontroller.cpp \
    $$PWD/qchangearbiter.cpp
