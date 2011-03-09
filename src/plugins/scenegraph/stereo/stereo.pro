TARGET  = qstereoscenegraph
include(../../qpluginbase.pri)

CONFIG += quick3d

SOURCES += \
    main.cpp \
    qsgstereocontext.cpp

HEADERS += \
    qsgstereocontext.h

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/scenegraph
target.path += $$[QT_INSTALL_PLUGINS]/scenegraph
INSTALLS += target
