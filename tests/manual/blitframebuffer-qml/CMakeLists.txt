# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from blitframebuffer-qml.pro.

#####################################################################
## blitframebuffer-qml Binary:
#####################################################################

qt_internal_add_manual_test(blitframebuffer-qml
    GUI
    SOURCES
        main.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DInput
        Qt::3DLogic
        Qt::3DQuick
        Qt::3DQuickExtras
        Qt::3DRender
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(blitframebuffer-qml_resource_files
    "main.qml"
)

qt_internal_add_resource(blitframebuffer-qml "blitframebuffer-qml"
    PREFIX
        "/"
    FILES
        ${blitframebuffer-qml_resource_files}
)


#### Keys ignored in scope 1:.:.:blitframebuffer-qml.pro:<TRUE>:
# OTHER_FILES = "main.qml"

## Scopes:
#####################################################################
