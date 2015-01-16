TEMPLATE = app

QT += 3dcore 3drenderer 3dbulletphysics 3dquick qml quick

include("../exampleresources/exampleresources.pri")

HEADERS += \

SOURCES += \
    main.cpp

OTHER_FILES += \
    main.qml \
    AdsEffect.qml \
    Renderable.qml \
    SimpleEffect.qml

RESOURCES += \
    rollerball.qrc
