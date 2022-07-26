# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

# Generated from transparency-qml.pro.

#####################################################################
## transparency-qml Binary:
#####################################################################

qt_internal_add_manual_test(transparency-qml
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
set(transparency-qml_resource_files
    "main.qml"
)

qt_internal_add_resource(transparency-qml "transparency-qml"
    PREFIX
        "/"
    FILES
        ${transparency-qml_resource_files}
)


## Scopes:
#####################################################################
