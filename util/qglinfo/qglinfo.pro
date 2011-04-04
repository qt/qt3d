TARGET = qglinfo
SOURCES += main.cpp \
    qglinfowindow.cpp \
    qglinfo.cpp \
    aboutdialog.cpp \
    fpswidget.cpp

package {
    LIBS += -L../../src/threed -lQt3D
    INCLUDEPATH += ../../include
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

package {
    target.path = $$[QT_INSTALL_BINS]
    INSTALLS += target
}
