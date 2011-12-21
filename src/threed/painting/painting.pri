
INCLUDEPATH += $$PWD
VPATH += $$PWD

HEADERS += \
    painting/qglabstracteffect.h \
    painting/qgllightmodel.h \
    painting/qgllightparameters.h \
    painting/qglpainter.h \
    painting/qmatrix4x4stack.h

SOURCES += \
    qglabstracteffect.cpp \
    qglext.cpp \
    qgllightmodel.cpp \
    qgllightparameters.cpp \
    qglpainter.cpp \
    qglpickcolors.cpp \
    qmatrix4x4stack.cpp

PRIVATE_HEADERS += \
    qglpainter_p.h \
    qglpickcolors_p.h \
    qglabstracteffect_p.h \
    qmatrix4x4stack_p.h \
    qglext_p.h
