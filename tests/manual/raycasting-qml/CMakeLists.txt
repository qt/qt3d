# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from raycasting-qml.pro.

#####################################################################
## raycasting-qml Binary:
#####################################################################

qt_internal_add_manual_test(raycasting-qml
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
set(raycasting-qml_resource_files
    main.qml
    CylinderEntity.qml
    LineEntity.qml
    LineMaterial.qml
)
set(raycasting-shader_resource_files
    shaders/es2/simpleColor.frag
    shaders/es2/simpleColor.vert
    shaders/gl3/simpleColor.frag
    shaders/gl3/simpleColor.vert
)

qt_internal_add_resource(raycasting-qml "raycasting-qml"
    PREFIX
        "/"
    FILES
        ${raycasting-qml_resource_files}
)

qt_internal_add_resource(raycasting-qml "shaders"
    PREFIX
        "/"
    FILES
        ${raycasting-shader_resource_files}
)
target_sources(raycasting-qml PRIVATE ${raycasting-qml_resource_files} ${raycasting-shader_resource_files})

#### Keys ignored in scope 1:.:.:raycasting-qml.pro:<TRUE>:
# OTHER_FILES = "main.qml"

## Scopes:
#####################################################################
