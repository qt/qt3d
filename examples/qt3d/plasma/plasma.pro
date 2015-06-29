!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += 3dcore 3drenderer 3dquick qml quick

HEADERS += \

SOURCES += \
    main.cpp

OTHER_FILES += \
    main.qml \
    plasma.frag \
    plasma.vert

RESOURCES += \
    plasma.qrc
