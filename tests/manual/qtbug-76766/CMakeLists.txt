# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from qtbug-76766.pro.

#####################################################################
## qtbug-76766 Binary:
#####################################################################

qt_internal_add_manual_test(qtbug-76766
    GUI
    SOURCES
        main.cpp
    LIBRARIES
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(qml_resource_files
    "FrameGraph.qml"
    "Material1.qml"
    "Material2.qml"
    "PostProcess.qml"
    "SceneRoot.qml"
    "expected_output.png"
    "main.qml"
)

qt_internal_add_resource(qtbug-76766 "qml"
    PREFIX
        "/"
    FILES
        ${qml_resource_files}
)
set(shaders_resource_files
    "shaders/shader.frag"
    "shaders/shader.vert"
)

qt_internal_add_resource(qtbug-76766 "shaders"
    PREFIX
        "/"
    FILES
        ${shaders_resource_files}
)


#### Keys ignored in scope 1:.:.:qtbug-76766.pro:<TRUE>:
# TEMPLATE = "app"
