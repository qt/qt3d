TEMPLATE = app

QT += 3dcore 3drenderer 3dquick 3dinput qml quick

include("../exampleresources/exampleresources.pri")

HEADERS += \

SOURCES += \
    main.cpp

OTHER_FILES += \
    shaders/* \
    main.qml \
    BasicCamera.qml \
    ForwardRenderer.qml \
    Wave.qml \
    WaveEffect.qml \
    WaveMaterial.qml \
    Background.qml \
    shaders/background.vert \
    shaders/background.frag

RESOURCES += \
    wave.qrc
