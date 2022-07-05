# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

# Generated from boundingvolumes.pro.

#####################################################################
## boundingvolumes Binary:
#####################################################################

qt_internal_add_manual_test(boundingvolumes
    GUI
    SOURCES
        main.cpp
    PUBLIC_LIBRARIES
        Qt::3DInput
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(boundingvolumes_resource_files
    "AnimatedEntity.qml"
    "main.qml"
)

qt_internal_add_resource(boundingvolumes "boundingvolumes"
    PREFIX
        "/"
    FILES
        ${boundingvolumes_resource_files}
)


#### Keys ignored in scope 1:.:.:boundingvolumes.pro:<TRUE>:
# OTHER_FILES = "AnimatedEntity.qml" "main.qml"

## Scopes:
#####################################################################
