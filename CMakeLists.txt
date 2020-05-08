# Generated from qt3d.pro.

cmake_minimum_required(VERSION 3.15.0)

project(Qt3D # special case
    VERSION 6.0.0
    DESCRIPTION "Qt 3D Libraries" # special case
    HOMEPAGE_URL "https://qt.io/"
    LANGUAGES CXX C
)

find_package(Qt6 ${PROJECT_VERSION} CONFIG REQUIRED COMPONENTS BuildInternals Core Gui
                                           OPTIONAL_COMPONENTS Concurrent Widgets Qml Quick Gamepad OpenGL OpenGLExtensions ShaderTools
)

if(NOT TARGET Qt::Gui)
    message(FATAL_ERROR "Skipping the build as the condition \"TARGET Qt::Gui\" is not met.")
    return()
endif()
if(WASM OR NOT QT_FEATURE_opengl)
    message(FATAL_ERROR "Skipping the build as the condition \"QT_FEATURE_opengl AND NOT WASM\" is not met.")
    return()
endif()

qt_build_repo()
