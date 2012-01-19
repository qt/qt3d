INCLUDEPATH += $$PWD
VPATH += $$PWD
HEADERS += \
    textures/qgltexture2d.h \
    textures/qgltexturecube.h \
    textures/qareaallocator.h
SOURCES += \
    qareaallocator.cpp \
    qgltexture2d.cpp \
    qgltexturecube.cpp \
    qgltextureutils.cpp
PRIVATE_HEADERS += \
    qgltexture2d_p.h \
    qgltextureutils_p.h


