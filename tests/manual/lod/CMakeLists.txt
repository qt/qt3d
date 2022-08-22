# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from lod.pro.

#####################################################################
## lod Binary:
#####################################################################

qt_internal_add_manual_test(lod
    GUI
    SOURCES
        main.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DInput
        Qt::3DQuick
        Qt::3DQuickExtras
        Qt::3DRender
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(lod_resource_files
    "ConeEntity.qml"
    "CuboidEntity.qml"
    "CylinderEntity.qml"
    "SphereEntity.qml"
    "main.qml"
)

qt_internal_add_resource(lod "lod"
    PREFIX
        "/"
    FILES
        ${lod_resource_files}
)


#### Keys ignored in scope 1:.:.:lod.pro:<TRUE>:
# DISTFILES = "ConeEntity.qml" "CuboidEntity.qml" "CylinderEntity.qml" "SphereEntity.qml"
# OTHER_FILES = "main.qml"

## Scopes:
#####################################################################
