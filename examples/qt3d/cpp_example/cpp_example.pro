!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += 3dcore 3drenderer 3dinput

SOURCES += main.cpp


RESOURCES += \
    ../exampleresources/test_scene.qrc \
    ../exampleresources/gltf.qrc
