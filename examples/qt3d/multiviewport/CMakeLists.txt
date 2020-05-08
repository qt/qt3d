# Generated from multiviewport.pro.

cmake_minimum_required(VERSION 3.14)
project(multiviewport LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(INSTALL_EXAMPLEDIR "examples")

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Qml)
find_package(Qt6 COMPONENTS Quick)

add_qt_gui_executable(multiviewport
    main.cpp
)
target_link_libraries(multiviewport PUBLIC
    Qt::3DCore
    Qt::3DInput
    Qt::3DQuick
    Qt::3DQuickExtras
    Qt::3DRender
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
)


# Resources:
set(multiviewport_resource_files
    "Gear_scene.dae"
    "QuadViewportFrameGraph.qml"
    "SimpleCamera.qml"
    "main.qml"
)

qt6_add_resources(multiviewport "multiviewport"
    PREFIX
        "/"
    FILES
        ${multiviewport_resource_files}
)
set_source_files_properties("../exampleresources/assets/test_scene.dae"
    PROPERTIES QT_RESOURCE_ALIAS "assets/test_scene.dae"
)
set(test_scene_resource_files
    "assets/test_scene.dae"
)

qt6_add_resources(multiviewport "test_scene"
    PREFIX
        "/"
    BASE
        "../exampleresources"
    FILES
        ${test_scene_resource_files}
)

install(TARGETS multiviewport
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
