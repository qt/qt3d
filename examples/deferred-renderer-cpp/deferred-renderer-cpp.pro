TEMPLATE = app

QT += 3dcore 3drenderer

include("../exampleresources/exampleresources.pri")

HEADERS += \
    gbuffer.h \
    deferredrenderer.h \
    finaleffect.h \
    sceneeffect.h

SOURCES += \
    main.cpp \
    gbuffer.cpp \
    deferredrenderer.cpp \
    finaleffect.cpp \
    sceneeffect.cpp

RESOURCES += \
    deferred-renderer-cpp.qrc

OTHER_FILES += \
    geometry_gl2.vert \
    geometry_gl2.frag \
    geometry_gl3.frag \
    geometry_gl3.vert \
    final_gl2.vert \
    final_gl2.frag \
    final_gl3.frag \
    final_gl3.vert
