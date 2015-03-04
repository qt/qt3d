TEMPLATE = app

SOURCE += main.cpp

QT += qml quick 3dcore 3drenderer 3dinput 3dquick

OTHER_FILES += main.qml

SOURCES += \
    main.cpp

include("../exampleresources/exampleresources.pri")

RESOURCES += \
    assimp.qrc
