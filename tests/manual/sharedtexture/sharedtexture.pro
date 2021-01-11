!include( ../manual.pri ) {
    error( "Couldn't find the manual.pri file!" )
}

QT += widgets 3dcore 3drender 3dinput 3dextras multimedia opengl

greaterThan(QT_MAJOR_VERSION, 5) {
    QT += openglwidgets
}

SOURCES += \
    videoplayer.cpp \
    main.cpp

HEADERS += \
    videoplayer.h
