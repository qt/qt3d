# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from simple-shaders-qml.pro.

#####################################################################
## simple-shaders-qml Binary:
#####################################################################

qt_internal_add_manual_test(simple-shaders-qml
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
set(simple-shaders-qml_resource_files
    "main.qml"
    "simple-shader.frag"
    "simple-shader.vert"
)

qt_internal_add_resource(simple-shaders-qml "simple-shaders-qml"
    PREFIX
        "/"
    FILES
        ${simple-shaders-qml_resource_files}
)


#### Keys ignored in scope 1:.:.:simple-shaders-qml.pro:<TRUE>:
# OTHER_FILES = "main.qml" "simple-shader.vert" "simple-shader.frag"

## Scopes:
#####################################################################
