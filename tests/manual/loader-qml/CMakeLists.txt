# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from loader-qml.pro.

#####################################################################
## loader-qml Binary:
#####################################################################

qt_internal_add_manual_test(loader-qml
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
set(loader-qml_resource_files
    "CuboidEntity.qml"
    "SphereEntity.qml"
    "main.qml"
)

qt_internal_add_resource(loader-qml "loader-qml"
    PREFIX
        "/"
    FILES
        ${loader-qml_resource_files}
)


#### Keys ignored in scope 1:.:.:loader-qml.pro:<TRUE>:
# OTHER_FILES = "main.qml" "SphereEntity.qml" "CuboidEntity.qml"

## Scopes:
#####################################################################
