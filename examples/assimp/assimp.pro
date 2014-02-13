TEMPLATE = app

SOURCE += main.cpp

QT += qml quick 3dcore

OTHER_FILES += main.qml

SOURCES += \
    main.cpp

include("../exampleresources/exampleresources.pri")

RESOURCES += \
    assimp.qrc
