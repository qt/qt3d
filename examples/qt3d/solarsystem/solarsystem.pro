TEMPLATE = app
TARGET = solarsystem
CONFIG += qt warn_on
CONFIG += qt3d

SOURCES = solarsystem.cpp \
    main.cpp
HEADERS = solarsystem.h
RESOURCES += solarsystem.qrc
DEPENDPATH += resources

package {
    maemo {
        applnk.files = builder.desktop
        applnk.path = /usr/share/applications

        icons.files = icon-l-qt3d.png
        icons.path = /usr/share/themes/base/meegotouch/icons
        INSTALLS += icons applnk
    }
    macx:CONFIG(qt_framework, qt_framework|qt_no_framework) {
        LIBS += -framework Qt3D -F../../../src/threed
        INCLUDEPATH += ../../../src/threed/Qt3D.framework/Versions/1/Headers
    } else {
        win32 {
            CONFIG(debug, debug|release) {
                TARGET = $$member(TARGET, 0)d
                LIBS += ..\\..\\..\\src\\threed\\debug\\Qt3Dd.lib
            } else {
                LIBS += ..\\..\\..\\src\\threed\\release\\Qt3D.lib
            }
        } else {
            LIBS += -L../../../src/threed -lQt3D
        }
        INCLUDEPATH += ../../../include/Qt3D
    }
    QT += opengl
    target.path += $$[QT_INSTALL_BINS]
    INSTALLS += target
} else {
    DESTDIR = ../../../bin
}

OTHER_FILES += \
    resources/moon-texture.jpg \
    resources/planet.jpg \
    resources/solar.jpg \
    resources/solar.fsh \
    resources/solar.vsh
