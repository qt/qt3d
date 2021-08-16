# Generated from scene2d.pro.

cmake_minimum_required(VERSION 3.16)
project(scene2d LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
  set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}")

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Qml)
find_package(Qt6 COMPONENTS Quick)
find_package(Qt6 COMPONENTS 3DQuick)
find_package(Qt6 COMPONENTS 3DQuickExtras)

qt_add_executable(scene2dsample # special case
    main.cpp
)
set_target_properties(scene2dsample PROPERTIES # special case
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_link_libraries(scene2dsample PUBLIC # special case
    Qt::3DQuick
    Qt::3DQuickExtras
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
)


# Resources:
set(scene2d_resource_files
    "LogoControls.qml"
    "Qt_logo.obj"
    "main.qml"
)

qt6_add_resources(scene2dsample "scene2d" # special case
    PREFIX
        "/"
    FILES
        ${scene2d_resource_files}
)

install(TARGETS scene2dsample # special case
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
