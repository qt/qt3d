!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += qml quick 3dcore 3drenderer 3dinput

SOURCES += \
    main.cpp

OTHER_FILES += \
    AnimatedEntity.qml \
    main.qml \
    Scene.qml \
    Scene2.qml

RESOURCES += \
    scene3d-loader.qrc
