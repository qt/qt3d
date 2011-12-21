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

# This is the *desktop* opengl library, which is not supported on some
# smaller footprint systems.  If this is turned off, then (for example)
# pixel-buffer objects are not supported.
!isEmpty(QT.opengl.name) {
    SOURCES += qglpixelbuffersurface.cpp
    HEADERS += surfaces/qglpixelbuffersurface.h
}

PRIVATE_HEADERS += \
    qglcontextsurface_p.h \
    qgldrawbuffersurface_p.h \
    qglmaskedsurface_p.h \
    qglpaintersurface_p.h



