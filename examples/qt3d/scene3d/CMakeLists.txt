# Generated from scene3d.pro.

cmake_minimum_required(VERSION 3.16)
project(scene3d LANGUAGES CXX)

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
find_package(Qt6 COMPONENTS 3DInput)
find_package(Qt6 COMPONENTS 3DRender)

qt_add_executable(scene3d
    main.cpp
)
set_target_properties(scene3d PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_link_libraries(scene3d PUBLIC
    Qt::3DInput
    Qt::3DRender
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
)


# Resources:
set(scene3d_resource_files
    "AnimatedEntity.qml"
    "main.qml"
)

qt6_add_resources(scene3d "scene3d"
    PREFIX
        "/"
    FILES
        ${scene3d_resource_files}
)

# special case begin
target_sources(scene3d PRIVATE ${scene3d_resource_files})
# special case end

install(TARGETS scene3d
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
