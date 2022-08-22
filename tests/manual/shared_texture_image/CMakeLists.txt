# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from shared_texture_image.pro.

#####################################################################
## shared_texture_image Binary:
#####################################################################

qt_internal_add_manual_test(shared_texture_image
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
set(shared_texture_image_resource_files
    "image.png"
    "main.qml"
)

qt_internal_add_resource(shared_texture_image "shared_texture_image"
    PREFIX
        "/"
    FILES
        ${shared_texture_image_resource_files}
)


#### Keys ignored in scope 1:.:.:shared_texture_image.pro:<TRUE>:
# OTHER_FILES = "main.qml"
