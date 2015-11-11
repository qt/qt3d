!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += 3dcore 3drender 3dinput 3dquick qml quick

HEADERS += \

SOURCES += \
    main.cpp

OTHER_FILES += \
    main.qml \
    Wine.qml

QT3D_MODELS = ../exampleresources/assets/gltf/wine/wine.dae
QGLTF_PARAMS = -g
load(qgltf)

RESOURCES += \
    qgltf_example.qrc
