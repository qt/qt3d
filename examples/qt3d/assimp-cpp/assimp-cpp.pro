android|ios|winrt {
    warning( "This example is not supported for android, ios, or winrt." )
}

!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += widgets 3dcore 3drenderer 3dinput

SOURCES += main.cpp
