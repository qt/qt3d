# Generated from advancedcustommaterial.pro.

cmake_minimum_required(VERSION 3.14)
project(advancedcustommaterial LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(INSTALL_EXAMPLEDIR "examples")

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Qml)
find_package(Qt6 COMPONENTS Quick)

add_qt_gui_executable(advancedcustommaterial
    main.cpp
)
target_link_libraries(advancedcustommaterial PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
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
target_sources(advancedcustommaterial PRIVATE ${qml_resource_files}) # just to show qml files in creator

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
