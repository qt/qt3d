!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += 3dcore 3drenderer 3dquick qml quick 3dinput

HEADERS += \

SOURCES += \
    main.cpp

OTHER_FILES += \
    main.qml \
    AdsEffect.qml \
    SimpleEffect.qml \
    elements/ViewportEntity.qml \
    elements/TechniqueFilterEntity.qml \
    elements/RenderPassFilterEntity.qml \
    elements/CameraSelectorEntity.qml \
    MyForwardRenderer.qml \
    ComplexTechnique.qml \
    RenderableEntity.qml \
    MainView.qml \
    DetailView.qml \
    AnimatedDiffuseMaterial.qml

RESOURCES += \
    playground-qml.qrc \
    ../exampleresources/gltf.qrc \
