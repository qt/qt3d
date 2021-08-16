# Generated from controls.pro.

cmake_minimum_required(VERSION 3.16)
project(controls LANGUAGES CXX)

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

qt_add_executable(controls
    main.cpp
)
set_target_properties(controls PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_link_libraries(controls PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Qml
    Qt::Quick
    Qt::3DRender
)


# Resources:
set(controls_resource_files
    "Logo.qml"
    "Qt_logo.obj"
    "main.qml"
)

qt6_add_resources(controls "controls"
    PREFIX
        "/"
    FILES
        ${controls_resource_files}
)

# special case begin
target_sources(controls PRIVATE ${controls_resources_files}) # just to show qml files in creator
# special case end

install(TARGETS controls
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
