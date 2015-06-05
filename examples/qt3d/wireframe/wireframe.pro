!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += 3dcore 3drenderer 3dinput 3dquick qml quick

HEADERS += \

SOURCES += \
    main.cpp

OTHER_FILES += \
    main.qml \
    BasicCamera.qml \
    WireframeEffect.qml \
    WireframeMaterial.qml \
    TrefoilKnot.qml

RESOURCES += \
    wireframe.qrc \
    ../exampleresources/obj.qrc
