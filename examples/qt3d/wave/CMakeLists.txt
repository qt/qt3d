# Generated from wave.pro.

cmake_minimum_required(VERSION 3.14)
project(wave LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(INSTALL_EXAMPLEDIR "examples")

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Qml)
find_package(Qt6 COMPONENTS Quick)

add_qt_gui_executable(wave
    main.cpp
)
target_link_libraries(wave PUBLIC
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
set(wave_resource_files
    "Background.qml"
    "BackgroundEffect.qml"
    "BasicCamera.qml"
    "Wave.qml"
    "WaveEffect.qml"
    "WaveForwardRenderer.qml"
    "WaveMaterial.qml"
    "main.qml"
    "shaders/background.frag"
    "shaders/background.vert"
    "shaders/ribbon.frag"
    "shaders/ribbon.vert"
    "shaders/ribbonwireframe.frag"
    "shaders/robustwireframe.geom"
)

qt6_add_resources(wave "wave"
    PREFIX
        "/"
    FILES
        ${wave_resource_files}
)

target_sources(wave PRIVATE ${wave_resource_files})

install(TARGETS wave
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)

