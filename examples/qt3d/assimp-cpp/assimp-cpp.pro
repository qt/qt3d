android|ios|tvos|winrt {
    warning( "This example is not supported for android, ios, tvos, or winrt." )
}

!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += widgets 3dcore 3drender 3dinput 3dextras

SOURCES += main.cpp
