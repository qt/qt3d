# Copyright (C) 2023 Klaralvdalens Datakonsult AB (KDAB).
# SPDX-License-Identifier: BSD-3-Clause

#####################################################################
## distancefieldtext-dynamic Binary:
#####################################################################

qt_internal_add_manual_test(distancefieldtext-dynamic
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
set(distancefieldtext-dynamic_resource_files
    "TextScene.qml"
    "main.qml"
)

qt_internal_add_resource(distancefieldtext-dynamic "distancefieldtext-dynamic"
    PREFIX
        "/"
    FILES
        ${distancefieldtext-dynamic_resource_files}
)

# OTHER_FILES = "TextScene.qml" "main.qml"

## Scopes:
#####################################################################
