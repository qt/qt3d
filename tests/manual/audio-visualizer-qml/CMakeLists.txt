# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(audio-visualizer-qml LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}")

find_package(Qt6 REQUIRED COMPONENTS 3DCore 3DQuick Core Gui Multimedia Qml Quick)

qt_add_executable(audio-visualizer-qml
    main.cpp
    touchsettings.cpp touchsettings.h
)

set_target_properties(audio-visualizer-qml PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(audio-visualizer-qml PUBLIC
    Qt::3DCore
    Qt::3DQuick
    Qt::Core
    Qt::Gui
    Qt::Multimedia
    Qt::Qml
    Qt::Quick
)

# Resources:
set(audio-visualizer-qml_resource_files
    "BarEntity.qml"
    "Visualizer.qml"
    "images/albumcover.png"
    "images/demotitle.png"
    "images/normalmap.png"
    "images/pausehoverpressed.png"
    "images/pausenormal.png"
    "images/playhoverpressed.png"
    "images/playnormal.png"
    "images/songtitle.png"
    "images/stopdisabled.png"
    "images/stophoverpressed.png"
    "images/stopnormal.png"
    "main.qml"
    "meshes/circle.obj"
    "meshes/progressbar.obj"
    "music/tiltshifted_lost_neon_sun.mp3"
    "music/visualization.raw"
)

qt6_add_resources(audio-visualizer-qml "audio-visualizer-qml"
    PREFIX
        "/"
    FILES
        ${audio-visualizer-qml_resource_files}
)

install(TARGETS audio-visualizer-qml
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
