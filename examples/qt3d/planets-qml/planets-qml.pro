!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += qml quick 3dinput

SOURCES += \
    main.cpp

OTHER_FILES += \
    *.qml \
    planets.js \
    shaders\*.*

RESOURCES += \
    planets-qml.qrc \
    planets-qml-images.qrc

DISTFILES += \
    PlanetsMain.qml
