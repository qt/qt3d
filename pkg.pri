!CONFIG(pkg_pri_included) {
CONFIG+=pkg_pri_included

# Config for making example and demo apps packageable

# Note that the paths here all assumed the including .pro file
# is exactly 3 levels of directory tree below the root

# Include this file after qt3d_pkg_dep.pri or qt3dquick_pkg_dep.pri

qtc_harmattan {
    CONFIG += maemo
    CONFIG += package
    QMAKE_CXXFLAGS += -Wno-psabi
    # The Qt SDK / Qt Creator harmattan integration needs some special treatment
    QT3D_INSTALL_BINS = /bin
    QT3D_INSTALL_LIBS = /usr/lib
    QT3D_INSTALL_PLUGINS = /usr/lib/qt4/plugins
    QT3D_INSTALL_IMPORTS = /usr/lib/qt4/imports
    QT3D_INSTALL_DATA = /usr/share/qt4
} else {
    QT3D_INSTALL_BINS = $$[QT_INSTALL_BINS]
    QT3D_INSTALL_LIBS = $$[QT_INSTALL_LIBS]
    QT3D_INSTALL_PLUGINS = $$[QT_INSTALL_PLUGINS]
    QT3D_INSTALL_IMPORTS = $$[QT_INSTALL_IMPORTS]
    QT3D_INSTALL_DATA = $$[QT_INSTALL_DATA]
}

contains(TEMPLATE, app) {
    package  {
        maemo {
            applnk.files = $${TARGET}.desktop
            applnk.path = /usr/share/applications

            # icons.files is set by qt3dquick_pkg_dep.pri or qt3d_pkg_dep.pri
            icons.path = /usr/share/themes/base/meegotouch/icons
            INSTALLS += icons applnk
        }
        target.path += $$QT3D_INSTALL_BINS
        INSTALLS += target
    } else {
        DESTDIR = ../../../bin
    }
}
}
