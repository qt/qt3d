# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from rendercapture-qml-fbo.pro.

#####################################################################
## rendercapture-qml-fbo Binary:
#####################################################################

qt_internal_add_manual_test(rendercapture-qml-fbo
    GUI
    SOURCES
        main.cpp
        rendercaptureprovider.h
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
set(qml_resource_files
    "CaptureScene.qml"
    "main.qml"
)

qt_internal_add_resource(rendercapture-qml-fbo "qml"
    PREFIX
        "/"
    FILES
        ${qml_resource_files}
)


#### Keys ignored in scope 1:.:.:rendercapture-qml-fbo.pro:<TRUE>:
# DISTFILES = "main.qml"

## Scopes:
#####################################################################
