# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from spritegrid.pro.

#####################################################################
## spritegrid Binary:
#####################################################################

qt_internal_add_manual_test(spritegrid
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
set(spritegrid_resource_files
    "main.qml"
    "spritegrid.png"
)

qt_internal_add_resource(spritegrid "spritegrid"
    PREFIX
        "/"
    FILES
        ${spritegrid_resource_files}
)


#### Keys ignored in scope 1:.:.:spritegrid.pro:<TRUE>:
# OTHER_FILES = "main.qml"

## Scopes:
#####################################################################
