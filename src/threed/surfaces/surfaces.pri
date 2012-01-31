INCLUDEPATH += $$PWD

VPATH += $$PWD

HEADERS += \
    surfaces/qglabstractsurface.h \
    surfaces/qglframebufferobjectsurface.h \
    surfaces/qglsubsurface.h \
    surfaces/qglwindowsurface.h

SOURCES += \
    qglabstractsurface.cpp \
    qglcontextsurface.cpp \
    qgldrawbuffersurface.cpp \
    qglframebufferobjectsurface.cpp \
    qglmaskedsurface.cpp \
    qglpaintersurface.cpp \
    qglsubsurface.cpp \
    qglwindowsurface.cpp

PRIVATE_HEADERS += \
    qglcontextsurface_p.h \
    qgldrawbuffersurface_p.h \
    qglmaskedsurface_p.h \
    qglpaintersurface_p.h



