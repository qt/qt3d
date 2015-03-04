TEMPLATE = app

QT += 3dcore 3drenderer 3dinput

include("../exampleresources/exampleresources.pri")

HEADERS += \
    planeentity.h \
    renderableentity.h \
    trefoilknot.h \
    barrel.h \
    rotatingtrefoilknot.h \
    houseplant.h

SOURCES += \
    main.cpp \
    planeentity.cpp \
    renderableentity.cpp \
    trefoilknot.cpp \
    barrel.cpp \
    rotatingtrefoilknot.cpp \
    houseplant.cpp
