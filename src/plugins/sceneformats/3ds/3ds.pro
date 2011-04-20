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

# See the README in the root dir re this code
package {
    macx:CONFIG(qt_framework, qt_framework|qt_no_framework) {
        LIBS += -framework Qt3D -F../../../threed
        INCLUDEPATH += ../../../threed/Qt3D.framework/Versions/1/Headers
    } else {
        win32 {
            CONFIG(debug, debug|release) {
                TARGET = $$member(TARGET, 0)d
                LIBS += ..\\..\\..\\threed\\debug\\Qt3Dd.lib
            } else {
                LIBS += ..\\..\\..\\threed\\release\\Qt3D.lib
            }
        } else {
            LIBS += -L../../../threed -lQt3D
        }
        INCLUDEPATH += ../../../../include/Qt3D
    }
    target.path = $$[QT_INSTALL_PLUGINS]/sceneformats]
    INSTALLS += target
} else {
    CONFIG += qt3d
    DESTDIR = $$[QT_INSTALL_PLUGINS]/sceneformats
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
