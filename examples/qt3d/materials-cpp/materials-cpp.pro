TEMPLATE = app

QT += 3dcore 3drenderer 3dinput

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

RESOURCES += \
    ../exampleresources/chest.qrc \
    ../exampleresources/houseplants.qrc \
    ../exampleresources/metalbarrel.qrc \
    ../exampleresources/obj.qrc \
    ../exampleresources/textures.qrc
