TARGET = qglinfo
SOURCES += main.cpp \
    qglinfowindow.cpp \
    qglinfo.cpp \
    aboutdialog.cpp \
    fpswidget.cpp

package {
    macx:CONFIG(qt_framework, qt_framework|qt_no_framework) {
        LIBS += -framework Qt3D -F../../src/threed
        INCLUDEPATH += ../../src/threed/Qt3D.framework/Versions/1/Headers
    } else {
        win32 {
            CONFIG(debug, debug|release) {
                LIBS += ..\\..\\src\\threed\\debug\\Qt3Dd.lib
            } else {
                LIBS += ..\\..\\src\\threed\\release\\Qt3D.lib
            }
        } else {
            LIBS += -L../../src/threed -lQt3D
        }
        INCLUDEPATH += ../../include/Qt3D
    }
    target.path += $$[QT_INSTALL_BINS]
    INSTALLS += target
    QT += opengl
} else {
    CONFIG += qt3d
    DESTDIR = ../../bin
}

CONFIG += qt3d warn_on
FORMS += qglinfowindow.ui \
    aboutdialog.ui
HEADERS += qglinfowindow.h \
    qglinfo.h \
    aboutdialog.h \
    fpswidget.h
RESOURCES += qglinfo.qrc

!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL

OTHER_FILES += \
    qglinfo.rc

RC_FILE = qglinfo.rc
