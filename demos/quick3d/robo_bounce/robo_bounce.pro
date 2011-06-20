TEMPLATE = app
TARGET = robo_bounce
CONFIG += qt warn_on

SOURCES += main.cpp
HEADERS += ../qmlres.h

QT += declarative

!package:DESTDIR = ../../../bin
# for cleanup on Windows platforms - avoid deletion prompts
win32 {
    QMAKE_DEL_FILE = del /q
    QMAKE_DEL_DIR = rmdir /s /q
}

package {
    maemo {
        applnk.files = robo_bounce.desktop
        applnk.path = /usr/share/applications

        icons.files = icon-l-qtquick3d.png
        icons.path = /usr/share/themes/base/meegotouch/icons
        INSTALLS += icons applnk
    }

    target.path = $$[QT_INSTALL_BINS]
    INSTALLS += target
}

OTHER_FILES += \
    robo_bounce.rc

RC_FILE = robo_bounce.rc

RESOURCES += \
    robo_bounce.qrc

