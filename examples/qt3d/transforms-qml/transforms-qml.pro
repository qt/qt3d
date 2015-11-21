!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += 3dcore 3drender 3dinput 3dquick qml quick

SOURCES += \
    main.cpp

OTHER_FILES += \
    main.qml \
    RenderableEntity.qml \
    TemporaryCamera.qml \
    TrefoilKnot.qml

RESOURCES += \
    transforms-qml.qrc \
    ../exampleresources/obj.qrc
