# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from animation-keyframe-blendtree.pro.

#####################################################################
## animation-keyframe-blendtree Binary:
#####################################################################

qt_internal_add_manual_test(animation-keyframe-blendtree
    GUI
    SOURCES
        main.cpp
    LIBRARIES
        Qt::3DQuickAnimation
        Qt::3DQuickExtras
        Qt::Gui
)

# Resources:
set(main_resource_files
    "DefaultSceneEntity.qml"
    "main.qml"
    "sliding-cube.json"
    "sliding-pulsing-cube.json"
)

qt_internal_add_resource(animation-keyframe-blendtree "main"
    PREFIX
        "/"
    FILES
        ${main_resource_files}
)


#### Keys ignored in scope 1:.:.:animation-keyframe-blendtree.pro:<TRUE>:
# OTHER_FILES = "main.qml" "DefaultSceneEntity.qml"
# TEMPLATE = "app"
