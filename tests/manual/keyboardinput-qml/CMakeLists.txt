# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from keyboardinput-qml.pro.

#####################################################################
## keyboardinput-qml Binary:
#####################################################################

qt_internal_add_manual_test(keyboardinput-qml
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
set(keyboardinput-qml_resource_files
    "SphereEntity.qml"
    "main.qml"
)

qt_internal_add_resource(keyboardinput-qml "keyboardinput-qml"
    PREFIX
        "/"
    FILES
        ${keyboardinput-qml_resource_files}
)


#### Keys ignored in scope 1:.:.:keyboardinput-qml.pro:<TRUE>:
# OTHER_FILES = "main.qml" "SphereEntity.qml"

## Scopes:
#####################################################################
