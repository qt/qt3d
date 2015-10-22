!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += 3dcore 3drender 3dinput 3dquick quick

SOURCES += \
    main.cpp

RESOURCES += \
    custom-mesh-qml.qrc
