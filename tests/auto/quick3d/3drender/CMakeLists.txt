# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from 3drender.pro.

#####################################################################
## tst_import3drender Test:
#####################################################################

qt_internal_add_test(tst_import3drender
    SOURCES
        ../../render/qmlscenereader/qmlscenereader.cpp ../../render/qmlscenereader/qmlscenereader.h
        tst_import3drender.cpp
    INCLUDE_DIRECTORIES
        ../../render/qmlscenereader
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DQuick
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::Gui
        Qt::Qml
)

# Resources:
set( 3drender_resource_files
    "3drender.qml"
)

qt_internal_add_resource(tst_import3drender "3drender"
    PREFIX
        "/"
    FILES
        ${3drender_resource_files}
)


#### Keys ignored in scope 1:.:.:3drender.pro:<TRUE>:
# TEMPLATE = "app"
