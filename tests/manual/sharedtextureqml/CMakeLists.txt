# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from sharedtextureqml.pro.

#####################################################################
## sharedtextureqml Binary:
#####################################################################

qt_internal_add_manual_test(sharedtextureqml
    GUI
    SOURCES
        ../sharedtexture/videoplayer.cpp ../sharedtexture/videoplayer.h
        main.cpp
    INCLUDE_DIRECTORIES
        ../sharedtexture
    LIBRARIES
        Qt::3DCore
        Qt::3DExtras
        Qt::3DInput
        Qt::3DQuickExtras
        Qt::3DRender
        Qt::Gui
        Qt::GuiPrivate
        Qt::Multimedia
        Qt::Quick
        Qt::Widgets
)

# Resources:
set(qml_resource_files
    "main.qml"
)

qt_internal_add_resource(sharedtextureqml "qml"
    PREFIX
        "/"
    FILES
        ${qml_resource_files}
)


## Scopes:
#####################################################################
