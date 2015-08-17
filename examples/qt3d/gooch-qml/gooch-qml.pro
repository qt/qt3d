!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += 3dcore 3drenderer 3dquick 3dinput qml quick

SOURCES += \
    main.cpp

OTHER_FILES += \
    main.qml \
    MyEntity.qml

RESOURCES += \
    gooch-qml.qrc \
    ../exampleresources/obj.qrc
