TARGET = qscene3ds
include(../../qpluginbase.pri)
HEADERS += qgl3dsloader.h \
    qgl3dsscene.h \
    qgl3dsscenehandler.h \
    qgl3dsmesh.h
SOURCES += main.cpp \
    qgl3dsloader.cpp \
    qgl3dsscene.cpp \
    qgl3dsscenehandler.cpp \
    qgl3dsmesh.cpp

package {
    LIBS += -L../../../threed
    INCLUDEPATH += ../../../../include
} else {
    CONFIG += qt3d
}

system_3ds {
    !isEmpty(QMAKE_INCDIR_3DS):INCLUDEPATH += $$QMAKE_INCDIR_3DS
    !isEmpty(QMAKE_LIBDIR_3DS):LIBS += -L$$QMAKE_LIBDIR_3DS

    !isEmpty(QMAKE_LIBS_3DS):LIBS += -l$$QMAKE_LIBS_3DS
    else {
        win32:LIBS += -llib3ds-1_3
        else:LIBS += -l3ds
    }
} else {
    include(../../../../3rdparty/lib3ds/lib3ds.pri)
}

target.path += $$[QT_INSTALL_PLUGINS]/sceneformats
INSTALLS += target
