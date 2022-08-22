# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from additional-attributes-qml.pro.

#####################################################################
## additional-attributes-qml Binary:
#####################################################################

qt_internal_add_manual_test(additional-attributes-qml
    GUI
    SOURCES
        main.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DExtras
        Qt::3DLogic
        Qt::3DQuick
        Qt::3DQuickExtras
        Qt::3DRender
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(additional-attributes-qml_resource_files
    "main.qml"
)

qt_internal_add_resource(additional-attributes-qml "additional-attributes-qml"
    PREFIX
        "/"
    FILES
        ${additional-attributes-qml_resource_files}
)


#### Keys ignored in scope 1:.:.:additional-attributes-qml.pro:<TRUE>:
# OTHER_FILES = "main.qml"

## Scopes:
#####################################################################
