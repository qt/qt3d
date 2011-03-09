
INCLUDEPATH += $$PWD
VPATH += $$PWD

HEADERS += \
    qglabstracteffect.h \
    qgllightmodel.h \
    qgllightparameters.h \
    qglpainter.h \
    qmatrix4x4stack.h

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
    qmatrix4x4stack_p.h

# Don't add this file to PRIVATE_HEADERS because we don't
# want moc to run over it:
#    qglext_p.h
