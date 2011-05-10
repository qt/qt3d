INCLUDEPATH += $$PWD
VPATH += $$PWD
HEADERS += \
    textures/qgltexture2d.h \
    textures/qgltexturecube.h \
    textures/qareaallocator.h
SOURCES += \
    qareaallocator.cpp \
    qglsharedresource.cpp \
    qgltexture2d.cpp \
    qgltexturecube.cpp \
    qgltextureutils.cpp
PRIVATE_HEADERS += \
    qglsharedresource_p.h \
    qgltexture2d_p.h
