cmake_minimum_required(VERSION 3.16)
project(scene2d LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}")

find_package(Qt6 REQUIRED COMPONENTS 3DQuick 3DQuickExtras Core Gui Qml Quick)

qt_add_executable(scene2dsample
    main.cpp
)

set_target_properties(scene2dsample PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(scene2dsample PUBLIC
    Qt::3DQuick
    Qt::3DQuickExtras
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
)

# Resources:
set(scene2d_resource_files
    "LogoControls.qml"
    "Qt_logo.obj"
    "main.qml"
)

qt6_add_resources(scene2dsample "scene2d"
    PREFIX
        "/"
    FILES
        ${scene2d_resource_files}
)

install(TARGETS scene2dsample
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
