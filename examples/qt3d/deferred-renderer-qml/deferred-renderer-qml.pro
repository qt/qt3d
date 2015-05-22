!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += 3dcore 3drenderer 3dinput 3dquick qml quick

HEADERS += \


OTHER_FILES += \
    main.qml \
    DeferredRenderer.qml \
    FinalEffect.qml \
    SceneEffect.qml \
    GBuffer.qml

RESOURCES += \
    deferred-renderer-qml.qrc

SOURCES += \
    main.cpp
