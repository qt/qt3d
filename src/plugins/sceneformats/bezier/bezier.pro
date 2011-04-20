TARGET  = qscenebezier
include(../../qpluginbase.pri)

HEADERS += qglbezierscene.h \
           qglbezierscenehandler.h
SOURCES += main.cpp \
           qglbezierscene.cpp \
           qglbezierscenehandler.cpp

# See the README in the root dir re this code
package {
    macx:CONFIG(qt_framework, qt_framework|qt_no_framework) {
        LIBS += -framework Qt3D -F../threed
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
    target.path = $$[QT_INSTALL_PLUGINS]/sceneformats
    INSTALLS += target
} else {
    CONFIG += qt3d
    DESTDIR = $$[QT_INSTALL_PLUGINS]/sceneformats
}
