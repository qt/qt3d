!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += 3dcore 3drender 3dquick qml 3dquickextras

SOURCES += \
    main.cpp

RESOURCES += \
    enabled-qml.qrc

