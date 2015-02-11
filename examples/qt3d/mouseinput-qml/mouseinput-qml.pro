!include( ../examples.pri ) {
    error( "Couldn't find the examples.pri file!" )
}

QT += 3dcore 3drenderer 3dinput 3dquick qml quick

SOURCES += main.cpp

RESOURCES += \
    mouseinput-qml.qrc

OTHER_FILES += \
    main.qml
