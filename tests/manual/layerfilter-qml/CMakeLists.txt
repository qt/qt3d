# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from layerfilter-qml.pro.

#####################################################################
## layerfilter-qml Binary:
#####################################################################

qt_internal_add_manual_test(layerfilter-qml
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
set(layerfilter-qml_resource_files
    "main.qml"
)

qt_internal_add_resource(layerfilter-qml "layerfilter-qml"
    PREFIX
        "/"
    FILES
        ${layerfilter-qml_resource_files}
)


#### Keys ignored in scope 1:.:.:layerfilter-qml.pro:<TRUE>:
# OTHER_FILES = "main.qml"

## Scopes:
#####################################################################
