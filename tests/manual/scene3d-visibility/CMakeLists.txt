# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from scene3d-visibility.pro.

#####################################################################
## scene3d-visibility Binary:
#####################################################################

qt_internal_add_manual_test(scene3d-visibility
    GUI
    SOURCES
        main.cpp
    LIBRARIES
        Qt::3DInput
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(scene3d-visibility_resource_files
    "main.qml"
)

qt_internal_add_resource(scene3d-visibility "scene3d-visibility"
    PREFIX
        "/"
    FILES
        ${scene3d-visibility_resource_files}
)


#### Keys ignored in scope 1:.:.:scene3d-visibility.pro:<TRUE>:
# OTHER_FILES = "main.qml"
