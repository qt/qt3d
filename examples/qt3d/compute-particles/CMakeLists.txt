# Generated from compute-particles.pro.

cmake_minimum_required(VERSION 3.16)
project(compute-particles LANGUAGES CXX)

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
find_package(Qt6 COMPONENTS 3DQuick)
find_package(Qt6 COMPONENTS 3DInput)
find_package(Qt6 COMPONENTS Quick)
find_package(Qt6 COMPONENTS Qml)

qt_add_executable(compute-particles
    main.cpp
)
set_target_properties(compute-particles PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
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
    "shaders/gl43/particles.comp"
    "shaders/gl43/particles.frag"
    "shaders/gl43/particles.vert"
    "shaders/gl45/particles.comp"
    "shaders/gl45/particles.frag"
    "shaders/gl45/particles.vert"
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
