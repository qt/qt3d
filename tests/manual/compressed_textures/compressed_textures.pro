QT += 3dcore 3drender 3dinput 3dquick 3dlogic qml quick 3dquickextras

SOURCES += \
    main.cpp

OTHER_FILES += \
    main.qml

RESOURCES += \
    compressed_textures.qrc

DEFINES += DATA_DIR=\\\"$$PWD/data\\\"
