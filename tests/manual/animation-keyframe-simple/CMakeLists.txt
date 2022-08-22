# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from animation-keyframe-simple.pro.

#####################################################################
## animation-keyframe-simple Binary:
#####################################################################

qt_internal_add_manual_test(animation-keyframe-simple
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
    "cubeanimation.json"
    "main.qml"
    "pulsing-cube-additive.json"
    "pulsing-moving-cube.json"
)

qt_internal_add_resource(animation-keyframe-simple "main"
    PREFIX
        "/"
    FILES
        ${main_resource_files}
)
target_sources(animation-keyframe-simple PRIVATE ${main_resource_files})

#### Keys ignored in scope 1:.:.:animation-keyframe-simple.pro:<TRUE>:
# OTHER_FILES = "main.qml" "DefaultSceneEntity.qml"
# TEMPLATE = "app"
