# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from deferred-renderer-qml.pro.

#####################################################################
## deferred-renderer-qml Binary:
#####################################################################

qt_internal_add_manual_test(deferred-renderer-qml
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
set(deferred-renderer-qml_resource_files
    "DeferredRenderer.qml"
    "FinalEffect.qml"
    "GBuffer.qml"
    "GBufferDebugger.qml"
    "SceneEffect.qml"
    "SceneEntity.qml"
    "ScreenQuadEntity.qml"
    "debug_es2.frag"
    "debug_es2.vert"
    "debug_gl3.frag"
    "debug_gl3.vert"
    "final_es2.frag"
    "final_es2.vert"
    "final_gl3.frag"
    "final_gl3.vert"
    "main.qml"
)

qt_internal_add_resource(deferred-renderer-qml "deferred-renderer-qml"
    PREFIX
        "/"
    FILES
        ${deferred-renderer-qml_resource_files}
)


#### Keys ignored in scope 1:.:.:deferred-renderer-qml.pro:<TRUE>:
# OTHER_FILES = "main.qml" "DeferredRenderer.qml" "FinalEffect.qml" "SceneEffect.qml" "SceneEntity.qml" "ScreenQuadEntity.qml" "GBuffer.qml"

## Scopes:
#####################################################################
