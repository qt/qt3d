# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from texture_property_updates.pro.

#####################################################################
## texture_property_updates Binary:
#####################################################################

qt_internal_add_manual_test(texture_property_updates
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
set(texture_property_updates_resource_files
    "image.jpg"
    "image2.jpg"
    "main.qml"
)

qt_internal_add_resource(texture_property_updates "texture_property_updates"
    PREFIX
        "/"
    FILES
        ${texture_property_updates_resource_files}
)


#### Keys ignored in scope 1:.:.:texture_property_updates.pro:<TRUE>:
# OTHER_FILES = "main.qml"
