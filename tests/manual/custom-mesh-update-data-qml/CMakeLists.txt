# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from custom-mesh-update-data-qml.pro.

#####################################################################
## custom-mesh-update-data-qml Binary:
#####################################################################

qt_internal_add_manual_test(custom-mesh-update-data-qml
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
        Qt::Quick
)

# Resources:
set(custom-mesh-update-data-qml_resource_files
    "main.qml"
)

qt_internal_add_resource(custom-mesh-update-data-qml "custom-mesh-update-data-qml"
    PREFIX
        "/"
    FILES
        ${custom-mesh-update-data-qml_resource_files}
)


## Scopes:
#####################################################################
