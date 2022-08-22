# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from quickwidget-switch.pro.

#####################################################################
## quickwidget-switch Binary:
#####################################################################

qt_internal_add_manual_test(quickwidget-switch
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
        Qt::QuickWidgets
        Qt::Widgets
)

# Resources:
set(quickwidget-switch_resource_files
    "main.qml"
)

qt_internal_add_resource(quickwidget-switch "quickwidget-switch"
    PREFIX
        "/"
    FILES
        ${quickwidget-switch_resource_files}
)


#### Keys ignored in scope 1:.:.:quickwidget-switch.pro:<TRUE>:
# TEMPLATE = "app"
