TEMPLATE = app

QT += 3dcore 3drenderer qml quick

include("../exampleresources/exampleresources.pri")

HEADERS += \

SOURCES += \
    main.cpp

OTHER_FILES += \
    main.qml

RESOURCES += \
    gltf.qrc
