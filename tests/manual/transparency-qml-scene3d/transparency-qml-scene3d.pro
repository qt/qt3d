!include( ../manual.pri ) {
    error( "Couldn't find the manual.pri file!" )
}

QT += qml quick 3dcore 3drender 3dinput 3dquick

SOURCES += main.cpp

RESOURCES += transparency-qml-scene3d.qrc

