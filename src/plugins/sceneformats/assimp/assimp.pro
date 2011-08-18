TARGET = qsceneai
include(../../qpluginbase.pri)
include(../../../../pkg.pri)
HEADERS += qailoader.h \
    qaiscene.h \
    qaiscenehandler.h \
    qaimesh.h \
    ailoaderiostream.h \
    ailoaderiosystem.h
SOURCES += main.cpp \
    qailoader.cpp \
    qaiscene.cpp \
    qaiscenehandler.cpp \
    qaimesh.cpp \
    ailoaderiostream.cpp \
    ailoaderiosystem.cpp

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
    target.path = $$QT3D_INSTALL_PLUGINS/sceneformats
    INSTALLS += target
} else {
    CONFIG += qt3d
    DESTDIR = $$QT3D_INSTALL_PLUGINS/sceneformats
}

symbian {
    LIBS += -lQt3D$${QT_LIBINFIX}
    TARGET.UID3 = 0x20031E97
}
# suppress warnings about std::copy using unsafe parameters
win32: DEFINES +=_SCL_SECURE_NO_WARNINGS

system_ai {
    !isEmpty(QMAKE_INCDIR_AI):INCLUDEPATH += $$QMAKE_INCDIR_AI
    !isEmpty(QMAKE_LIBDIR_AI):LIBS += -L$$QMAKE_LIBDIR_AI

    !isEmpty(QMAKE_LIBS_AI):LIBS += -l$$QMAKE_LIBS_AI
    else {
        win32:LIBS += -llibai-1_3
        else:LIBS += -lai
    }
} else {
    include(../../../../3rdparty/assimp/assimp.pri)
}

