# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from distancefieldtext.pro.

#####################################################################
## distancefieldtext Binary:
#####################################################################

qt_internal_add_manual_test(distancefieldtext
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
set(distancefieldtext_resource_files
    "TextScene.qml"
    "main.qml"
)

qt_internal_add_resource(distancefieldtext "distancefieldtext"
    PREFIX
        "/"
    FILES
        ${distancefieldtext_resource_files}
)


#### Keys ignored in scope 1:.:.:distancefieldtext.pro:<TRUE>:
# OTHER_FILES = "TextScene.qml" "main.qml"

## Scopes:
#####################################################################
