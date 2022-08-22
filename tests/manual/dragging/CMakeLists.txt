# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from dragging.pro.

#####################################################################
## dragging Binary:
#####################################################################

qt_internal_add_manual_test(dragging
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
set(dragging_resource_files
    "main.qml"
)

qt_internal_add_resource(dragging "dragging"
    PREFIX
        "/"
    FILES
        ${dragging_resource_files}
)


#### Keys ignored in scope 1:.:.:dragging.pro:<TRUE>:
# OTHER_FILES = "main.qml"

## Scopes:
#####################################################################
