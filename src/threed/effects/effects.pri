
INCLUDEPATH += $$PWD
VPATH += $$PWD

HEADERS += \
    qglshaderprogrameffect.h \
    qglcolladafxeffectfactory.h \
    qglcolladafxeffect.h \
    qglcolladafxeffectloader.h

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
