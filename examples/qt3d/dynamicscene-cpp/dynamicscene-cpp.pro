!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += 3dcore 3drenderer 3dinput

SOURCES += \
    main.cpp \
    examplescene.cpp \
    forwardrenderer.cpp \
    boxentity.cpp

HEADERS += \
    examplescene.h \
    forwardrenderer.h \
    boxentity.h
