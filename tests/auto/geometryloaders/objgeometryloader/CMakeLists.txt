# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

# Generated from objgeometryloader.pro.

#####################################################################
## tst_objgeometryloader Test:
#####################################################################

qt_internal_add_test(tst_objgeometryloader
    SOURCES
        tst_objgeometryloader.cpp
    PUBLIC_LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::Gui
)

# Resources:
set(resources_resource_files
    "invalid_vertex_position.obj"
)

qt_internal_add_resource(tst_objgeometryloader "resources"
    PREFIX
        "/"
    FILES
        ${resources_resource_files}
)


#### Keys ignored in scope 1:.:.:objgeometryloader.pro:<TRUE>:
# OTHER_FILES = "invalid_vertex_position.obj"
# TEMPLATE = "app"
