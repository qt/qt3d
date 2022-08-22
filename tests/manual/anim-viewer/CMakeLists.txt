# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from anim-viewer.pro.

#####################################################################
## anim-viewer Binary:
#####################################################################

qt_internal_add_manual_test(anim-viewer
    GUI
    SOURCES
        main.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DExtras
        Qt::3DInput
        Qt::3DQuick
        Qt::3DQuickExtras
        Qt::3DRender
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(qml_resource_files
    "assets/blendshapeanimation.dae"
    "assets/gears.dae"
    "main.qml"
)

qt_internal_add_resource(anim-viewer "qml"
    PREFIX
        "/"
    FILES
        ${qml_resource_files}
)


#### Keys ignored in scope 1:.:.:anim-viewer.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################
