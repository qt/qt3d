# Generated from scene2d.pro.

cmake_minimum_required(VERSION 3.14)
project(scene2d LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(INSTALL_EXAMPLEDIR "examples")

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Qml)
find_package(Qt6 COMPONENTS Quick)

add_qt_gui_executable(scene2dsample
    main.cpp
)
target_link_libraries(scene2dsample PUBLIC
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

qt6_add_resources(scene2dsample "scene2d"
    PREFIX
        "/"
    FILES
        ${scene2d_resource_files}
)

install(TARGETS scene2dsample
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
