# See README for an explanation of package build mode
# Note that the paths here all assumed the including .pro file
# is exactly 3 levels of directory tree below the root

include(qt3d_pkg_dep.pri)

package {
    macx:CONFIG(qt_framework, qt_framework|qt_no_framework) {
        LIBS += -framework Qt3DQuick -F../../../src/quick3d
        INCLUDEPATH += ../../../src/quick3d/Qt3DQuick.framework/Versions/1/Headers
    } else {
        win32 {
            CONFIG(debug, debug|release) {
                TARGET = $$member(TARGET, 0)d
                LIBS += ..\\..\\..\\src\\quick3d\\debug\\Qt3DQuickd.lib
            } else {
                LIBS += ..\\..\\..\\src\\quick3d\\release\\Qt3DQuick.lib
            }
        } else {
            LIBS += -L../../../src/quick3d -lQt3DQuick
        }
        INCLUDEPATH += ../../../include/Qt3DQuick
    }
    QT += declarative opengl

    maemo: icons.files = icon-l-qtquick3d.png
}

symbian {
    ICON = ../qtquick3d.svg
}

include(pkg.pri)
