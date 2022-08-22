# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

#####################################################################
## ubo Binary:
#####################################################################

qt_internal_add_manual_test(ubo
    GUI
    SOURCES
        main.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DExtras
        Qt::3DInput
        Qt::3DRender
        Qt::3DQuick
        Qt::3DQuickExtras
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Qml)
find_package(Qt6 COMPONENTS Quick)

# Resources:
set(qmake_immediate_resource_files
    "main.qml"
)

qt_internal_add_resource(ubo "qmake_immediate"
    PREFIX
        "/"
    FILES
        ${qmake_immediate_resource_files}
)


## Scopes:
#####################################################################
