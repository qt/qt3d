# Generated from advancedcustommaterial.pro.

cmake_minimum_required(VERSION 3.16)
project(advancedcustommaterial LANGUAGES CXX)

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
find_package(Qt6 COMPONENTS 3DRender)

qt_add_executable(advancedcustommaterial
    main.cpp
)
set_target_properties(advancedcustommaterial PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_link_libraries(advancedcustommaterial PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    Qt::3DRender
)


# Resources:
set(qml_resource_files
    "SceneRoot.qml"
    "Water.qml"
    "WaterMaterial.qml"
    "main.qml"
)

qt6_add_resources(advancedcustommaterial "qml"
    PREFIX
        "/"
    FILES
        ${qml_resource_files}
)
# special case begin
target_sources(advancedcustommaterial PRIVATE ${qml_resource_files}) # just to show qml files in creator
# special case end

set(models_resource_files
    "models/waterPlane.obj"
)

qt6_add_resources(advancedcustommaterial "models"
    PREFIX
        "/"
    FILES
        ${models_resource_files}
)
set(textures_resource_files
    "textures/WaterDiffuse.jpg"
    "textures/WaterNormal.jpg"
    "textures/WaterSpecular.jpg"
    "textures/Waterwave.jpg"
    "textures/foam.jpg"
    "textures/sky.jpg"
)

qt6_add_resources(advancedcustommaterial "textures"
    PREFIX
        "/"
    FILES
        ${textures_resource_files}
)
set(shaders_resource_files
    "shaders/es2/water.frag"
    "shaders/es2/water.vert"
    "shaders/gl3/water.frag"
    "shaders/gl3/water.vert"
    "shaders/rhi/water.frag"
    "shaders/rhi/water.vert"
)

qt6_add_resources(advancedcustommaterial "shaders"
    PREFIX
        "/"
    FILES
        ${shaders_resource_files}
)

install(TARGETS advancedcustommaterial
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
