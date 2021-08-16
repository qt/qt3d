# Generated from qardboard.pro.

cmake_minimum_required(VERSION 3.16)
project(qardboard LANGUAGES CXX)

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
find_package(Qt6 COMPONENTS 3DCore)
find_package(Qt6 COMPONENTS 3DRender)
find_package(Qt6 COMPONENTS 3DInput)
find_package(Qt6 COMPONENTS 3DQuick)
find_package(Qt6 COMPONENTS 3DExtras)
find_package(Qt6 COMPONENTS 3DQuickExtras)

qt_add_executable(qardboard
    abstractdeviceorientation.cpp abstractdeviceorientation.h
    dummydeviceorientation.cpp dummydeviceorientation.h
    main.cpp
)
set_target_properties(qardboard PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_link_libraries(qardboard PUBLIC
    Qt::3DCore
    Qt::3DExtras
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
set(resources_resource_files
    "PlacedEntity.qml"
    "QardboardCamera.qml"
    "QardboardFrameGraph.qml"
    "QardboardRootEntity.qml"
    "QardboardScene3D.qml"
    "main.qml"
)

qt6_add_resources(qardboard "resources"
    PREFIX
        "/"
    FILES
        ${resources_resource_files}
)
set(obj_resource_files
    "../exampleresources/assets/obj/ball.obj"
    "../exampleresources/assets/obj/material-sphere.obj"
    "../exampleresources/assets/obj/plane-10x10.obj"
    "../exampleresources/assets/obj/toyplane.obj"
    "../exampleresources/assets/obj/trefoil.obj"
)

qt6_add_resources(qardboard "obj"
    PREFIX
        "/"
    BASE
        "../exampleresources"
    FILES
        ${obj_resource_files}
)

if(IOS)
    target_sources(qardboard PUBLIC
        iosdeviceorientation.h iosdeviceorientation.mm iosdeviceorientation_p.h iosdeviceorientation_p.mm
    )

    target_link_libraries(qardboard PUBLIC
        Qt::Sensors
        Qt::SensorsPrivate
    )
endif()

install(TARGETS qardboard
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
