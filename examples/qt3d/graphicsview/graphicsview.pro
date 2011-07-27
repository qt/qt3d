TEMPLATE = app
TARGET = graphicsview
CONFIG += qt warn_on
!package: CONFIG += qt3d

VPATH += $$PWD/../../src/threed/geometry
SOURCES = \
    cubeitem.cpp \
    modelitem.cpp \
    teapotitem.cpp \
    main.cpp \
    edge.cpp \
    node.cpp \
    graph.cpp \
    scene.cpp \
    view.cpp
HEADERS = \
    cubeitem.h \
    modelitem.h \
    teapotitem.h \
    edge.h \
    node.h \
    graph.h \
    scene.h \
    view.h
RESOURCES = \
    graphicsview.qrc

CONFIG += qt3d_deploy_pkg
include(../../../pkg.pri)

OTHER_FILES += \
    graphicsview.rc

RC_FILE = graphicsview.rc

symbian {
    ICON = ../qt3d.svg
}

symbian {
    ICON = ../qt3d.svg
}
