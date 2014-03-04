TEMPLATE = app

QT += 3dcore 3drenderer qml quick

include("../exampleresources/exampleresources.pri")

HEADERS += \

SOURCES += \
    main.cpp

OTHER_FILES += \
    main.qml \
    AdsEffect.qml \
    Renderable.qml \
    SimpleEffect.qml \
    elements/ViewportEntity.qml \
    elements/TechniqueFilterEntity.qml \
    elements/RenderPassFilterEntity.qml \
    elements/CameraSelectorEntity.qml \
    ForwardRenderer.qml

RESOURCES += \
    simple-qml.qrc
