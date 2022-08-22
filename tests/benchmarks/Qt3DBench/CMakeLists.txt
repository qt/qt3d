# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from qt3dbench-qml.pro.

#####################################################################
## qt3dbench-qml Binary:
#####################################################################

qt_internal_add_benchmark(qt3dbench-qml
    GUI
    SOURCES
        main.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DExtras
        Qt::3DQuick
        Qt::3DRender
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(qt3dbench-qml_resource_files
    "GroundPlane.qml"
    "Light.qml"
    "Qt3DBenchFrameGraph.qml"
    "Qt3DBenchMain.qml"
    "ShadowEffect.qml"
    "ShadowMaterial.qml"
    "SphereElement.qml"
    "SphereView.qml"
    "shaders/ads.frag"
    "shaders/ads.vert"
    "shaders/shadowmap.frag"
    "shaders/shadowmap.vert"
)

qt_internal_add_resource(qt3dbench-qml "qt3dbench-qml"
    PREFIX
        "/"
    FILES
        ${qt3dbench-qml_resource_files}
)


#### Keys ignored in scope 1:.:.:qt3dbench-qml.pro:<TRUE>:
# OTHER_FILES = "*.qml"

## Scopes:
#####################################################################
