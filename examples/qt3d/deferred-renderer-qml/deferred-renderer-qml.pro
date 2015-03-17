TEMPLATE = app

QT += 3dcore 3drenderer 3dinput 3dquick qml quick

include("../exampleresources/exampleresources.pri")

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
