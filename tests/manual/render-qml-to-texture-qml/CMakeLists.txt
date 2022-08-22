# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from render-qml-to-texture-qml.pro.

#####################################################################
## render-qml-to-texture-qml Binary:
#####################################################################

qt_internal_add_manual_test(render-qml-to-texture-qml
    GUI
    SOURCES
        main.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DExtras
        Qt::3DInput
        Qt::3DQuick
        Qt::3DRender
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(render-qml-to-texture-qml_resource_files
    "InteractiveGui.qml"
    "main.qml"
)

qt_internal_add_resource(render-qml-to-texture-qml "render-qml-to-texture-qml"
    PREFIX
        "/"
    FILES
        ${render-qml-to-texture-qml_resource_files}
)


#### Keys ignored in scope 1:.:.:render-qml-to-texture-qml.pro:<TRUE>:
# OTHER_FILES = "main.qml"

## Scopes:
#####################################################################
