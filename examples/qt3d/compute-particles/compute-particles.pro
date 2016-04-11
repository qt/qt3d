!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += 3dcore 3drender 3dquick 3dinput quick qml 3dextras

HEADERS += \


RESOURCES += \
    compute-particles.qrc

SOURCES += \
    main.cpp
