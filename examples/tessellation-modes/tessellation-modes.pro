TEMPLATE = app

QT += 3dcore 3drenderer 3dquick qml quick

include("../exampleresources/exampleresources.pri")

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
    ForwardRenderer.qml \
    shaders/*

RESOURCES += \
    tessellation-modes.qrc
