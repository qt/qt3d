TEMPLATE = app

QT += 3dcore 3drenderer

include("../exampleresources/exampleresources.pri")

SOURCES += \
    main.cpp \
    examplescene.cpp \
    forwardrenderer.cpp \
    boxentity.cpp

HEADERS += \
    examplescene.h \
    forwardrenderer.h \
    boxentity.h
