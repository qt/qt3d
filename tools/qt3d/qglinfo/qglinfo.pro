TARGET = qglinfo
QT += 3d widgets opengl
!build_pass:contains(QT_CONFIG, build_all): CONFIG += release

SOURCES += main.cpp \
    qglinfowindow.cpp \
    qglinfo.cpp \
    aboutdialog.cpp \
    fpswidget.cpp

FORMS += qglinfowindow.ui \
    aboutdialog.ui
HEADERS += qglinfowindow.h \
    qglinfo.h \
    aboutdialog.h \
    fpswidget.h
RESOURCES += qglinfo.qrc

# This assumes the legacy (non-QPA) EGL support, which is gone for a while now.
# !contains(QT_CONFIG, egl):
DEFINES += QT_NO_EGL

OTHER_FILES += \
    qglinfo.rc

RC_FILE = qglinfo.rc

DESTDIR = $$QT.3d.bins
target.path = $$[QT_INSTALL_BINS]
INSTALLS += target
