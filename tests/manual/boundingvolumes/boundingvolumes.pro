!include( ../manual.pri ) {
    error( "Couldn't find the manual.pri file!" )
}

QT += qml quick 3dinput

SOURCES += \
    main.cpp

OTHER_FILES += \
    AnimatedEntity.qml \
    main.qml

RESOURCES += \
    boundingvolumes.qrc
