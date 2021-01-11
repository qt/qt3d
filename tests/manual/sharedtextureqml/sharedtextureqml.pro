!include( ../manual.pri ) {
    error( "Couldn't find the manual.pri file!" )
}

QT += widgets gui-private 3dcore 3drender 3dinput 3dextras multimedia quick 3dquickextras

greaterThan(QT_MAJOR_VERSION, 5) {
    QT += openglwidgets
}

SOURCES += \
    main.cpp \
    ../sharedtexture/videoplayer.cpp

HEADERS += \
    ../sharedtexture/videoplayer.h

INCLUDEPATH += ../sharedtexture

RESOURCES += \
    qml.qrc
