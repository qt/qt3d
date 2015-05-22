!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += 3dcore 3drenderer 3dinput 3dquick qml quick

HEADERS += \

SOURCES += \
    main.cpp

OTHER_FILES += \
    main.qml \
    *.qml \
    HousePlant.qml

RESOURCES += \
    materials.qrc \
    ../exampleresources/chest.qrc \
    ../exampleresources/houseplants.qrc \
    ../exampleresources/metalbarrel.qrc \
    ../exampleresources/obj.qrc \
    ../exampleresources/textures.qrc
