# Generated from compute-particles.pro.

cmake_minimum_required(VERSION 3.14)
project(compute-particles LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(INSTALL_EXAMPLEDIR "examples")

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Quick)
find_package(Qt6 COMPONENTS Qml)

add_qt_gui_executable(compute-particles
    main.cpp
)
target_link_libraries(compute-particles PUBLIC
    Qt::3DCore
    Qt::3DInput
    Qt::3DQuick
    Qt::3DRender
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
)


# Resources:
set(compute-particles_resource_files
    "ComputeFrameGraph.qml"
    "ComputeMaterial.qml"
    "ParticlesScene.qml"
    "main.qml"
    "particles.comp"
    "particles.frag"
    "particles.vert"
)

qt6_add_resources(compute-particles "compute-particles"
    PREFIX
        "/"
    FILES
        ${compute-particles_resource_files}
)

install(TARGETS compute-particles
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
