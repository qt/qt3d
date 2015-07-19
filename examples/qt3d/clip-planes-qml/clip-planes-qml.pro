!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += qml quick 3dcore 3drenderer 3dinput 3dquick

SOURCES += main.cpp

RESOURCES += \
    clip-planes-qml.qrc \
    ../exampleresources/obj.qrc
