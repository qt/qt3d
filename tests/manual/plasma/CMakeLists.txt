# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from plasma.pro.

#####################################################################
## plasma Binary:
#####################################################################

qt_internal_add_manual_test(plasma
    GUI
    SOURCES
        main.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DQuick
        Qt::3DQuickExtras
        Qt::3DRender
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(plasma_resource_files
    "main.qml"
    "plasma.frag"
    "plasma.vert"
)

qt_internal_add_resource(plasma "plasma"
    PREFIX
        "/"
    FILES
        ${plasma_resource_files}
)


#### Keys ignored in scope 1:.:.:plasma.pro:<TRUE>:
# OTHER_FILES = "main.qml" "plasma.frag" "plasma.vert"

## Scopes:
#####################################################################
