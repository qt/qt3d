cmake_minimum_required(VERSION 3.16)
project(qgltf LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}")

find_package(Qt6 REQUIRED COMPONENTS 3DCore 3DInput 3DQuick 3DQuickExtras 3DRender Core Gui Qml Quick)

qt_add_executable(qgltf
    main.cpp
)

set_target_properties(qgltf PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(qgltf PUBLIC
    Qt::3DCore
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
set(qgltf_example_resource_files
    "Scene.qml"
    "main.qml"
)

qt6_add_resources(qgltf "qgltf_example"
    PREFIX
        "/"
    FILES
        ${qgltf_example_resource_files}
)

install(TARGETS qgltf
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
