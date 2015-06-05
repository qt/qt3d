!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += 3dcore 3drenderer 3dinput 3dquick qml quick

HEADERS += \
    tessellatedquadmesh.h

SOURCES += \
    main.cpp \
    tessellatedquadmesh.cpp

OTHER_FILES += \
    main.qml \
    BasicCamera.qml \
    TessellatedWireframeEffect.qml \
    TessellatedWireframeMaterial.qml \
    TessellatedQuad.qml \
    shaders/*

RESOURCES += \
    tessellation-modes.qrc
