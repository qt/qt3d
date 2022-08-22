# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from video-texture-qml.pro.

#####################################################################
## video-texture-qml Binary:
#####################################################################

qt_internal_add_manual_test(video-texture-qml
    GUI
    SOURCES
        main.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DInput
        Qt::3DQuick
        Qt::3DQuickExtras
        Qt::3DQuickRender
        Qt::3DRender
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(video-texture-qml_resource_files
    "main.qml"
)

qt_internal_add_resource(video-texture-qml "video-texture-qml"
    PREFIX
        "/"
    FILES
        ${video-texture-qml_resource_files}
)


#### Keys ignored in scope 1:.:.:video-texture-qml.pro:<TRUE>:
# DISTFILES = <EMPTY>
# OTHER_FILES = "main.qml"

## Scopes:
#####################################################################
