# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# Generated from boundingsphere.pro.

#####################################################################
## tst_boundingsphere Test:
#####################################################################

qt_internal_add_test(tst_boundingsphere
    SOURCES
        ../qmlscenereader/qmlscenereader.cpp ../qmlscenereader/qmlscenereader.h
        tst_boundingsphere.cpp
    INCLUDE_DIRECTORIES
        ../qmlscenereader
    LIBRARIES
        Qt::3DCore
        Qt::3DCorePrivate
        Qt::3DExtras
        Qt::3DQuick
        Qt::3DQuickPrivate
        Qt::3DQuickExtras
        Qt::3DRender
        Qt::3DRenderPrivate
        Qt::CorePrivate
        Qt::Gui
        Qt::Qml
        Qt::Quick
)

# Resources:
set(boundingsphere_resource_files
    "cube.qml"
    "sphere.qml"
)

qt_internal_add_resource(tst_boundingsphere "boundingsphere"
    PREFIX
        "/"
    FILES
        ${boundingsphere_resource_files}
)


#### Keys ignored in scope 1:.:.:boundingsphere.pro:<TRUE>:
# TEMPLATE = "app"

## Scopes:
#####################################################################

include(../commons/commons.cmake)
qt3d_setup_common_render_test(tst_boundingsphere)
