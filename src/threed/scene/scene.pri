INCLUDEPATH += $$PWD
VPATH += $$PWD
HEADERS += qglabstractscene.h \
    qglsceneformatplugin.h \
    qglscenenode.h \
    qglpicknode.h \
    qglrendersequencer.h \
    qglrenderorder.h \
    qglrenderordercomparator.h \
    qglrenderstate.h
SOURCES += qglabstractscene.cpp \
    qglsceneformatplugin.cpp \
    qglscenenode.cpp \
    qglpicknode.cpp \
    qglrendersequencer.cpp \
    qglrenderorder.cpp \
    qglrenderordercomparator.cpp \
    qglrenderstate.cpp
PRIVATE_HEADERS += qglscenenode_p.h
