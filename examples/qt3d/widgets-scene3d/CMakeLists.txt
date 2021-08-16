# Generated from widgets-scene3d.pro.

cmake_minimum_required(VERSION 3.16)
project(widgets-scene3d LANGUAGES CXX)

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
find_package(Qt6 COMPONENTS Widgets)
find_package(Qt6 COMPONENTS Qml)
find_package(Qt6 COMPONENTS Quick)
find_package(Qt6 COMPONENTS QuickWidgets)
find_package(Qt6 COMPONENTS 3DInput)

qt_add_executable(widgets-scene3d
    main.cpp
)
set_target_properties(widgets-scene3d PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_link_libraries(widgets-scene3d PUBLIC
    Qt::3DInput
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    Qt::QuickWidgets
    Qt::Widgets
)


# Resources:
set_source_files_properties("../scene3d/AnimatedEntity.qml"
    PROPERTIES QT_RESOURCE_ALIAS "AnimatedEntity.qml"
)
set_source_files_properties("../scene3d/main.qml"
    PROPERTIES QT_RESOURCE_ALIAS "main.qml"
)
set(widgets-scene3d_resource_files
    "../scene3d/AnimatedEntity.qml"
    "../scene3d/main.qml"
)

qt6_add_resources(widgets-scene3d "widgets-scene3d"
    PREFIX
        "/"
    FILES
        ${widgets-scene3d_resource_files}
)

install(TARGETS widgets-scene3d
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
