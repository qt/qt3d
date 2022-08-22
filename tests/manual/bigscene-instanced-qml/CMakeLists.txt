# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from bigscene-instanced-qml.pro.

#####################################################################
## bigscene-instanced-qml Binary:
#####################################################################

qt_internal_add_manual_test(bigscene-instanced-qml
    GUI
    SOURCES
        main.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DExtras
        Qt::3DInput
        Qt::3DQuick
        Qt::3DQuickExtras
        Qt::3DRender
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(bigscene-instanced-qml_resource_files
    "instanced.frag"
    "instanced.vert"
    "main.qml"
)

qt_internal_add_resource(bigscene-instanced-qml "bigscene-instanced-qml"
    PREFIX
        "/"
    FILES
        ${bigscene-instanced-qml_resource_files}
)


## Scopes:
#####################################################################
