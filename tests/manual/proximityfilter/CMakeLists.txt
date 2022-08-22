# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from proximityfilter.pro.

#####################################################################
## proximityfilter Binary:
#####################################################################

qt_internal_add_manual_test(proximityfilter
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
        Qt::Quick
)

# Resources:
set(proximityfilter_resource_files
    "main.qml"
)

qt_internal_add_resource(proximityfilter "proximityfilter"
    PREFIX
        "/"
    FILES
        ${proximityfilter_resource_files}
)


## Scopes:
#####################################################################
