# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from gltfplugins.pro.

#####################################################################
## tst_gltfplugins Test:
#####################################################################

qt_internal_add_test(tst_gltfplugins
    SOURCES
        tst_gltfplugins.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DExtras
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::Gui
)

# Resources:
set(images_resource_files
    "ontopmaterial.frag"
    "ontopmaterial.vert"
    "ontopmaterialES2.frag"
    "ontopmaterialES2.vert"
    "qtlogo.png"
    "qtlogo_normal.png"
    "qtlogo_specular.png"
    "qtlogo_with_alpha.png"
)

qt_internal_add_resource(tst_gltfplugins "images"
    PREFIX
        "/"
    FILES
        ${images_resource_files}
)


#### Keys ignored in scope 1:.:.:gltfplugins.pro:<TRUE>:
# TEMPLATE = "app"
