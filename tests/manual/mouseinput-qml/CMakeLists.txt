# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from mouseinput-qml.pro.

#####################################################################
## mouseinput-qml Binary:
#####################################################################

qt_internal_add_manual_test(mouseinput-qml
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
set(mouseinput-qml_resource_files
    "main.qml"
)

qt_internal_add_resource(mouseinput-qml "mouseinput-qml"
    PREFIX
        "/"
    FILES
        ${mouseinput-qml_resource_files}
)


#### Keys ignored in scope 1:.:.:mouseinput-qml.pro:<TRUE>:
# OTHER_FILES = "main.qml"

## Scopes:
#####################################################################
