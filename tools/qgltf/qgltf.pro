option(host_build)
!cross_compile:load(qt_build_paths)

SOURCES = qgltf.cpp

config_assimp:!cross_compile {
    !contains(QT_CONFIG, no-pkg-config) {
        CONFIG += link_pkgconfig
        PKGCONFIG_PRIVATE += assimp
    } else {
        LIBS += -lassimp
    }
} else {
    # Fix for using bootstrap module on OSX outside of qtbase
    contains(QT_CONFIG, qt_framework) {
        # Add framework headers directly to make bootstrap lib work
        INCLUDEPATH += $$QT.core.libs/QtCore.framework/Headers
        # Extend framework search path to make #include <QtCore/*> work
        QMAKE_CXXFLAGS += -F$$QT.core.libs
    }
    CONFIG += force_bootstrap

    include(../../src/3rdparty/assimp/assimp.pri)
}

load(qt_tool)
