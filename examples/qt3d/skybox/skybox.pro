TEMPLATE = app

QT += 3dcore 3drenderer 3dinput 3dquick qml quick

include("../exampleresources/exampleresources.pri")

RESOURCES += \
    skybox.qrc

SOURCES += \
    main.cpp
