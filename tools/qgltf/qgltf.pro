option(host_build)
!cross_compile:load(qt_build_paths)

SOURCES = qgltf.cpp

include(../../src/3rdparty/assimp/assimp_dependency.pri)

force_bootstrap {
    # Fix for using bootstrap module on OSX outside of qtbase
    contains(QT_CONFIG, qt_framework) {
        # Add framework headers directly to make bootstrap lib work
        INCLUDEPATH += $$QT.core.libs/QtCore.framework/Headers
        # Extend framework search path to make #include <QtCore/*> work
        QMAKE_CXXFLAGS += -F$$QT.core.libs
    }
}

load(qt_tool)
