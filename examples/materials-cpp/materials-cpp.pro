TEMPLATE = app

QT += 3dcore 3drenderer

include("../exampleresources/exampleresources.pri")

HEADERS += \
    planeentity.h \
    forwardrenderer.h \
    renderableentity.h \
    trefoilknot.h \
    barrel.h \
    rotatingtrefoilknot.h \
    houseplant.h

SOURCES += \
    main.cpp \
    planeentity.cpp \
    forwardrenderer.cpp \
    renderableentity.cpp \
    trefoilknot.cpp \
    barrel.cpp \
    rotatingtrefoilknot.cpp \
    houseplant.cpp
