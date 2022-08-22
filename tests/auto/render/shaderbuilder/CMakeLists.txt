# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from shaderbuilder.pro.

#####################################################################
## tst_shaderbuilder Test:
#####################################################################

qt_internal_add_test(tst_shaderbuilder
    SOURCES
        tst_shaderbuilder.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::Gui
)

# Resources:
set(shaderbuilder_resource_files
    "input.json"
    "lightmodel.es2.inc"
    "lightmodel.gl3.inc"
    "output.es2"
    "output.gl3"
    "prototypes.json"
)

qt_internal_add_resource(tst_shaderbuilder "shaderbuilder"
    PREFIX
        "/"
    FILES
        ${shaderbuilder_resource_files}
)


#### Keys ignored in scope 1:.:.:shaderbuilder.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(../commons/commons.cmake)
qt3d_setup_common_render_test(tst_shaderbuilder)
