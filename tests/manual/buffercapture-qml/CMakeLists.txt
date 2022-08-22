# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from buffercapture-qml.pro.

#####################################################################
## buffercapture-qml Binary:
#####################################################################

qt_internal_add_manual_test(buffercapture-qml
    GUI
    SOURCES
        main.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DExtras
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
set(resources_resource_files
    "BufferSetterScene.qml"
    "ComputeFrameGraph.qml"
    "ComputeMaterial.qml"
    "gl43/bufferSetter.comp"
    "gl45/bufferSetter.comp"
    "main.qml"
)

qt_internal_add_resource(buffercapture-qml "resources"
    PREFIX
        "/"
    FILES
        ${resources_resource_files}
)


#### Keys ignored in scope 1:.:.:buffercapture-qml.pro:<TRUE>:
# DISTFILES = "main.qml" "main.qml"

## Scopes:
#####################################################################
