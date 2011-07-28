TARGET = qglinfo

CONFIG += qt3d_deploy_pkg
include(../../../pkg.pri)

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

!contains(QT_CONFIG, egl):DEFINES += QT_NO_EGL

OTHER_FILES += \
    qglinfo.rc

RC_FILE = qglinfo.rc
