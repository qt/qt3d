# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from simple-rhi-cpp.pro.

#####################################################################
## simple-rhi-cpp Binary:
#####################################################################

qt_internal_add_manual_test(simple-rhi-cpp
    GUI
    SOURCES
        main.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DExtras
        Qt::3DInput
        Qt::3DRender
        Qt::Gui
)

# Resources:
set(qmake_immediate_resource_files
    "qtlogo.png"
)

qt_internal_add_resource(simple-rhi-cpp "qmake_immediate"
    PREFIX
        "/"
    FILES
        ${qmake_immediate_resource_files}
)


## Scopes:
#####################################################################
