# Generated from multiviewport.pro.

cmake_minimum_required(VERSION 3.16)
project(multiviewport LANGUAGES CXX)

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
find_package(Qt6 COMPONENTS 3DCore)
find_package(Qt6 COMPONENTS 3DRender)
find_package(Qt6 COMPONENTS 3DInput)
find_package(Qt6 COMPONENTS 3DQuick)
find_package(Qt6 COMPONENTS Qml)
find_package(Qt6 COMPONENTS Quick)
find_package(Qt6 COMPONENTS 3DQuickExtras)

qt_add_executable(multiviewport
    main.cpp
)
set_target_properties(multiviewport PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
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
set(test_scene_resource_files
    "../exampleresources/assets/test_scene.dae"
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
