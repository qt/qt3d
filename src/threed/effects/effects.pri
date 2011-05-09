
INCLUDEPATH += $$PWD
VPATH += $$PWD

HEADERS += \
    effects/qglshaderprogrameffect.h \
    effects/qglcolladafxeffectfactory.h \
    effects/qglcolladafxeffect.h \
    effects/qglcolladafxeffectloader.h

SOURCES += \
    qglflatcoloreffect.cpp \
    qglflattextureeffect.cpp \
    qgllitmaterialeffect.cpp \
    qgllittextureeffect.cpp \
    qglshaderprogrameffect.cpp \
    qglcolladafxeffect.cpp \
    qglcolladafxeffectfactory.cpp \
    qglcolladafxeffectloader.cpp

PRIVATE_HEADERS += \
    qglflatcoloreffect_p.h \
    qglflattextureeffect_p.h \
    qgllitmaterialeffect_p.h \
    qgllittextureeffect_p.h \
    qglcolladafxeffect_p.h
