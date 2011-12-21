
INCLUDEPATH += $$PWD
VPATH += $$PWD

HEADERS += \
    graphicsview/qgraphicsbillboardtransform.h \
    graphicsview/qgraphicsrotation3d.h \
    graphicsview/qgraphicsscale3d.h \
    graphicsview/qgraphicstransform3d.h \
    graphicsview/qgraphicstranslation3d.h

SOURCES += \
    qgraphicsbillboardtransform.cpp \
    qgraphicsrotation3d.cpp \
    qgraphicsscale3d.cpp \
    qgraphicstransform3d.cpp \
    qgraphicstranslation3d.cpp

# This requires desktop stuff
!isEmpty(QT.widgets.name): !isEmpty(QT.opengl.name) {
    HEADERS += \
        graphicsview/qglgraphicsviewportitem.h \
        graphicsview/qgraphicsembedscene.h
    SOURCES += \
        qglgraphicsviewportitem.cpp \
        qgraphicsembedscene.cpp
}
