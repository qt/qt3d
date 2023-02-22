# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from transparency-qml-scene3d.pro.

#####################################################################
## transparency-qml-scene3d Binary:
#####################################################################

qt_internal_add_manual_test(transparency-qml-scene3d
    GUI
    SOURCES
        main.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DInput
        Qt::3DQuick
        Qt::3DQuickExtras
        Qt::3DRender
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(transparency-qml-scene3d_resource_files
    "Scene.qml"
    "main.qml"
)

qt_internal_add_resource(transparency-qml-scene3d "transparency-qml-scene3d"
    PREFIX
        "/"
    FILES
        ${transparency-qml-scene3d_resource_files}
)
set(cubemaps_resource_files
    "../exampleresources/assets/cubemaps/default/default_irradiance.dds"
    "../exampleresources/assets/cubemaps/default/default_specular.dds"
    "../exampleresources/assets/cubemaps/miramar/miramar_negx.webp"
    "../exampleresources/assets/cubemaps/miramar/miramar_negy.webp"
    "../exampleresources/assets/cubemaps/miramar/miramar_negz.webp"
    "../exampleresources/assets/cubemaps/miramar/miramar_posx.webp"
    "../exampleresources/assets/cubemaps/miramar/miramar_posy.webp"
    "../exampleresources/assets/cubemaps/miramar/miramar_posz.webp"
)

qt_internal_add_resource(transparency-qml-scene3d "cubemaps"
    PREFIX
        "/"
    BASE
        "../exampleresources"
    FILES
        ${cubemaps_resource_files}
)


## Scopes:
#####################################################################
