# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from deferred-renderer-cpp.pro.

#####################################################################
## deferred-renderer-cpp Binary:
#####################################################################

qt_internal_add_manual_test(deferred-renderer-cpp
    GUI
    SOURCES
        deferredrenderer.cpp deferredrenderer.h
        finaleffect.cpp finaleffect.h
        gbuffer.cpp gbuffer.h
        main.cpp
        pointlightblock.cpp pointlightblock.h
        sceneeffect.cpp sceneeffect.h
        sceneentity.cpp sceneentity.h
        screenquadentity.cpp screenquadentity.h
    LIBRARIES
        Qt::3DCore
        Qt::3DExtras
        Qt::3DInput
        Qt::3DRender
        Qt::Gui
)

# Resources:
set(deferred-renderer-cpp_resource_files
    "final_gl2.frag"
    "final_gl2.vert"
    "final_gl3.frag"
    "final_gl3.vert"
    "geometry_gl2.frag"
    "geometry_gl2.vert"
    "geometry_gl3.frag"
    "geometry_gl3.vert"
)

qt_internal_add_resource(deferred-renderer-cpp "deferred-renderer-cpp"
    PREFIX
        "/"
    FILES
        ${deferred-renderer-cpp_resource_files}
)


#### Keys ignored in scope 1:.:.:deferred-renderer-cpp.pro:<TRUE>:
# OTHER_FILES = "geometry_gl2.vert" "geometry_gl2.frag" "geometry_gl3.frag" "geometry_gl3.vert" "final_gl2.vert" "final_gl2.frag" "final_gl3.frag" "final_gl3.vert"

## Scopes:
#####################################################################
