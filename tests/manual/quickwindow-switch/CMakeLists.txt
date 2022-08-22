# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from quickwindow-switch.pro.

#####################################################################
## quickwindow-switch Binary:
#####################################################################

qt_internal_add_manual_test(quickwindow-switch
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
set(quickwindow-switch_resource_files
    "main.qml"
)

qt_internal_add_resource(quickwindow-switch "quickwindow-switch"
    PREFIX
        "/"
    FILES
        ${quickwindow-switch_resource_files}
)


#### Keys ignored in scope 1:.:.:quickwindow-switch.pro:<TRUE>:
# TEMPLATE = "app"
