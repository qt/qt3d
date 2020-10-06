# Generated from qt3d.pro.

cmake_minimum_required(VERSION 3.15.0)

project(Qt3D # special case
    VERSION 6.0.0
    DESCRIPTION "Qt 3D Libraries" # special case
    HOMEPAGE_URL "https://qt.io/"
    LANGUAGES CXX C
)

# special case begin
# Make sure we only use latest private CMake API, aka no compatibility wrappers.
set(QT_NO_INTERNAL_COMPATIBILITY_FUNCTIONS TRUE)
# special case end

find_package(Qt6 ${PROJECT_VERSION} CONFIG REQUIRED COMPONENTS BuildInternals Core Gui
                                           OPTIONAL_COMPONENTS Concurrent Widgets Qml Quick Gamepad OpenGL ShaderTools
)

if(NOT TARGET Qt::Gui)
    message(FATAL_ERROR "Skipping the build as the condition \"TARGET Qt::Gui\" is not met.")
    return()
endif()
if(WASM OR NOT QT_FEATURE_opengl)
    message(FATAL_ERROR "Skipping the build as the condition \"QT_FEATURE_opengl AND NOT WASM\" is not met.")
    return()
endif()

# special case begin
# TODO: Fix warnings and remove this
set(QT_REPO_NOT_WARNINGS_CLEAN TRUE)
# special case end

qt_build_repo()
