TEMPLATE = app

QT += 3dcore 3drenderer 3dinput 3dquick qml quick

SOURCES += \
    main.cpp

OTHER_FILES += \
    main.qml \
    AdsMaterial.qml \
    AdsEffect.qml \
    Light.qml \
    ShadowMapFrameGraph.qml \
    Trefoil.qml \
    Toyplane.qml \
    GroundPlane.qml

RESOURCES += \
    shadow-map-qml.qrc \
    ../exampleresources/obj.qrc
