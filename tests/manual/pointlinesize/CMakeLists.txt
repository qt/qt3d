# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from pointlinesize.pro.

#####################################################################
## pointlinesize Binary:
#####################################################################

qt_internal_add_manual_test(pointlinesize
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
set(pointlinesize_resource_files
    "Scene.qml"
    "main.qml"
)

qt_internal_add_resource(pointlinesize "pointlinesize"
    PREFIX
        "/"
    FILES
        ${pointlinesize_resource_files}
)


#### Keys ignored in scope 1:.:.:pointlinesize.pro:<TRUE>:
# OTHER_FILES = "Scene.qml" "main.qml"

## Scopes:
#####################################################################
