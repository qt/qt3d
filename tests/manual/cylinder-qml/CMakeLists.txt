# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from cylinder-qml.pro.

#####################################################################
## cylinder-qml Binary:
#####################################################################

qt_internal_add_manual_test(cylinder-qml
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
set(cylinder-qml_resource_files
    "main.qml"
)

qt_internal_add_resource(cylinder-qml "cylinder-qml"
    PREFIX
        "/"
    FILES
        ${cylinder-qml_resource_files}
)


#### Keys ignored in scope 1:.:.:cylinder-qml.pro:<TRUE>:
# OTHER_FILES = "main.qml"

## Scopes:
#####################################################################
