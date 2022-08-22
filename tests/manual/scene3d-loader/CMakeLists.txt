# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from scene3d-loader.pro.

#####################################################################
## scene3d-loader Binary:
#####################################################################

qt_internal_add_manual_test(scene3d-loader
    GUI
    SOURCES
        main.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DInput
        Qt::3DRender
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(scene3d-loader_resource_files
    "AnimatedEntity.qml"
    "Scene.qml"
    "Scene2.qml"
    "main.qml"
)

qt_internal_add_resource(scene3d-loader "scene3d-loader"
    PREFIX
        "/"
    FILES
        ${scene3d-loader_resource_files}
)


#### Keys ignored in scope 1:.:.:scene3d-loader.pro:<TRUE>:
# OTHER_FILES = "AnimatedEntity.qml" "main.qml" "Scene.qml" "Scene2.qml"

## Scopes:
#####################################################################
