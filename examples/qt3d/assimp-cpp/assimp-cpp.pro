!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += widgets 3dcore 3drenderer 3dinput

SOURCES += main.cpp
