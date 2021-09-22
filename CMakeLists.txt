# Generated from qt3d.pro.

cmake_minimum_required(VERSION 3.16)

include(.cmake.conf)
project(Qt3D # special case
    VERSION "${QT_REPO_MODULE_VERSION}"
    DESCRIPTION "Qt 3D Libraries" # special case
    HOMEPAGE_URL "https://qt.io/"
    LANGUAGES CXX C
)

# special case begin
# Make sure we only use latest private CMake API, aka no compatibility wrappers.
set(QT_NO_INTERNAL_COMPATIBILITY_FUNCTIONS TRUE)

# Make sure we use the fixed BASE argument of qt_add_resource.
set(QT_USE_FIXED_QT_ADD_RESOURCE_BASE TRUE)

find_package(Qt6 ${PROJECT_VERSION} CONFIG REQUIRED COMPONENTS BuildInternals Core)
find_package(Qt6 ${PROJECT_VERSION} QUIET CONFIG OPTIONAL_COMPONENTS
             Concurrent Widgets Qml Quick Gamepad OpenGL Multimedia)
# special case end

if(NOT TARGET Qt::Gui)
    message(NOTICE "Skipping the build as the condition \"TARGET Qt::Gui\" is not met.")
    return()
endif()
if(WASM OR NOT QT_FEATURE_opengl)
    message(NOTICE "Skipping the build as the condition \"QT_FEATURE_opengl AND NOT WASM\" is not met.")
    return()
endif()

# special case begin
# TODO: Fix warnings and remove this
set(QT_REPO_NOT_WARNINGS_CLEAN TRUE)
# special case end

qt_build_repo()
