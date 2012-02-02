INCLUDEPATH += $$PWD
VPATH += $$PWD
HEADERS += scene/qglabstractscene.h \
    scene/qglsceneformatplugin.h \
    scene/qglscenenode.h \
    scene/qglpicknode.h \
    scene/qglrendersequencer.h \
    scene/qglrenderorder.h \
    scene/qglrenderordercomparator.h \
    scene/qglrenderstate.h \
    scene/qglsceneanimation.h
SOURCES += qglabstractscene.cpp \
    qglsceneformatplugin.cpp \
    qglscenenode.cpp \
    qglpicknode.cpp \
    qglrendersequencer.cpp \
    qglrenderorder.cpp \
    qglrenderordercomparator.cpp \
    qglrenderstate.cpp \
    scene/qglsceneanimation.cpp
PRIVATE_HEADERS += qglscenenode_p.h
