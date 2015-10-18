!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += 3dcore 3drender 3dinput 3dquick qml quick

RESOURCES += \
    skybox.qrc \
    ../exampleresources/cubemaps.qrc

SOURCES += \
    main.cpp
