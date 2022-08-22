# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from gltexture.pro.

#####################################################################
## tst_gltexture Test:
#####################################################################

qt_internal_add_test(tst_gltexture
    SOURCES
        tst_gltexture.cpp
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::Gui
)

# Resources:
set(resources_resource_files
    "image.jpg"
)

qt_internal_add_resource(tst_gltexture "resources"
    PREFIX
        "/"
    FILES
        ${resources_resource_files}
)


#### Keys ignored in scope 1:.:.:gltexture.pro:<TRUE>:
# TEMPLATE = "app"
