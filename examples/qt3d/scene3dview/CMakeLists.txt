# Generated from scene3dview.pro.

cmake_minimum_required(VERSION 3.14)
project(scene3dview LANGUAGES CXX)

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

qt_add_executable(scene3dview
    main.cpp
)
target_link_libraries(scene3dview PUBLIC
    Qt::3DInput
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
)


# Resources:
set(scene3dview_resource_files
    "AnimatedEntity.qml"
    "main.qml"
)

qt6_add_resources(scene3dview "scene3dview"
    PREFIX
        "/"
    FILES
        ${scene3dview_resource_files}
)

install(TARGETS scene3dview
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
